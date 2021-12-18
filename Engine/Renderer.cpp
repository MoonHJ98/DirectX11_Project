#include "pch.h"
#include "Renderer.h"
#include "GraphicDevice.h"
#include "GameObject.h"
#include "DeferredBuffer.h"
#include "LightManager.h"
#include "Shader.h"
#include "RectangleBuffer.h"
#include "Transform.h"
#include "Management.h"
#include "Frustum.h"


HRESULT Renderer::Initialize()
{
	Graphic = GraphicDevice::GetInstance();

	UINT viewportNum = 1;
	D3D11_VIEWPORT viewport;
	Graphic->GetDeviceContext()->RSGetViewports(&viewportNum, &viewport);
	
	LightMgr = LightManager::GetInstance();

	deferredBuffer = DeferredBuffer::Create(SCREENSIZEX, SCREENSIZEY, SCREENDEPTH, SCREENNEAR);

	deferredBuffer->AddMultiRenderTarget(L"Deferred", DeferredBuffer::DIFFUSEMAP);
	deferredBuffer->AddMultiRenderTarget(L"Deferred", DeferredBuffer::NORMALMAP);
	deferredBuffer->AddMultiRenderTarget(L"Deferred", DeferredBuffer::SPECULARMAP);
	deferredBuffer->AddMultiRenderTarget(L"Deferred", DeferredBuffer::DEPTH);
	deferredBuffer->AddMultiRenderTarget(L"Deferred", DeferredBuffer::LIGHTVIEWPOS);
	



	

	deferredBuffer->AddMultiRenderTarget(L"Light", DeferredBuffer::SHADE);
	deferredBuffer->AddMultiRenderTarget(L"Light", DeferredBuffer::SPECULAR);

	deferredBuffer->AddMultiRenderTarget(L"Shadow", DeferredBuffer::DEPTHFORSHADOW);

	deferredBuffer->AddMultiRenderTarget(L"Blend", DeferredBuffer::BLEND);

	//deferredBuffer->AddMultiRenderTarget(L"Light", DeferredBuffer::SHADOW);




	D3D11_INPUT_ELEMENT_DESC InputLayout[] =
	{
		{ "POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 0, D3D11_INPUT_PER_VERTEX_DATA, 0 },
		{ "TEXCOORD", 0, DXGI_FORMAT_R32G32_FLOAT, 0, D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_VERTEX_DATA, 0 }
	
	};

	lightShader = Shader::Create(InputLayout, sizeof(InputLayout), L"../Engine/LightVS.hlsl", L"../Engine/LightPS.hlsl");

	blendShader = Shader::Create(InputLayout, sizeof(InputLayout), L"../Engine/BlendVS.hlsl", L"../Engine/BlendPS.hlsl");

	rectangleBuffer = RectangleBuffer::Create();


	shared_ptr<ConstantBuffer<ProjtoWorldType>> temp(new ConstantBuffer<ProjtoWorldType>());
	projToWorld = temp;
	projToWorld->Create(Graphic->GetDevice());

	Manage = Management::GetInstance();
	frustum = Frustum::Create();
	return S_OK;
}

ID3D11ShaderResourceView * Renderer::GetBlendTexture()
{
	return deferredBuffer->GetShaderResourceView(DeferredBuffer::BLEND);
}

void Renderer::Render()
{
	RenderPriority();

	RenderNonAlpha();

	RenderAlpha();

	RenderUI();

	RenderLight();

	RenderBlend();
}

void Renderer::RenderPriority()
{
	for (auto& pGameObject : RenderGroup[RENDER_PRIORITY])
	{
		if (nullptr != pGameObject)
		{
			pGameObject->Render();
		}
	}

	RenderGroup[RENDER_PRIORITY].clear();
}

void Renderer::RenderNonAlpha()
{
	Matrix view = *Manage->GetTransform(D3DTRANSFORMSTATE_VIEW);
	Matrix proj = *Manage->GetTransform(D3DTRANSFORMSTATE_PROJECTION);

	frustum->UpdateFrustum(SCREENDEPTH, proj, view);

	deferredBuffer->BeginMRT(L"Shadow");
	
	for (auto& pGameObject : RenderGroup[RENDER_NONALPHA])
	{
		if (nullptr != pGameObject)
		{
			//if (frustum->CheckPoint(&pGameObject->GetPosition()))
			//{
				pGameObject->RenderDepthForShadow(true);
				pGameObject->Render();
			//}
		}
	}
	
	deferredBuffer->EndMRT();




	deferredBuffer->BeginMRT(L"Deferred");

	for (auto& pGameObject : RenderGroup[RENDER_NONALPHA])
	{
		if (nullptr != pGameObject)
		{
			//if (frustum->CheckPoint(&pGameObject->GetPosition()))
			//{
				pGameObject->RenderDepthForShadow(false);
				pGameObject->Render();
			//}
		}
	}

	deferredBuffer->EndMRT();


	RenderGroup[RENDER_NONALPHA].clear();

}

void Renderer::RenderAlpha()
{
}

void Renderer::RenderUI()
{


}

void Renderer::RenderLight()
{
	lightShader->Render();
	
	deferredBuffer->BeginMRT(L"Light");
	
	auto normal = deferredBuffer->GetShaderResourceView(DeferredBuffer::NORMALMAP);
	auto specular = deferredBuffer->GetShaderResourceView(DeferredBuffer::SPECULARMAP);
	auto depth = deferredBuffer->GetShaderResourceView(DeferredBuffer::DEPTH);
	auto shadow = deferredBuffer->GetShaderResourceView(DeferredBuffer::DEPTHFORSHADOW);
	auto lightViewPos = deferredBuffer->GetShaderResourceView(DeferredBuffer::LIGHTVIEWPOS);


	
	Graphic->GetDeviceContext()->PSSetShaderResources(0, 1, &normal);
	Graphic->GetDeviceContext()->PSSetShaderResources(1, 1, &specular);
	Graphic->GetDeviceContext()->PSSetShaderResources(2, 1, &depth);
	Graphic->GetDeviceContext()->PSSetShaderResources(3, 1, &shadow);
	Graphic->GetDeviceContext()->PSSetShaderResources(4, 1, &lightViewPos);




	ProjtoWorldType buffertype;
	Matrix View = *Manage->GetTransform(D3DTRANSFORMSTATE_VIEW);
	Matrix Proj = *Manage->GetTransform(D3DTRANSFORMSTATE_PROJECTION);
	Vector3 CamPos;


	View = XMMatrixInverse(nullptr, View);
	Proj = XMMatrixInverse(nullptr, Proj);

	buffertype.ProjInv = XMMatrixTranspose(Proj);
	buffertype.ViewInv = XMMatrixTranspose(View);
	buffertype.CamPos = Manage->FindGameObjectTest(0, L"Camera", L"Camera").get()->GetPosition();


	projToWorld->SetData(Graphic->GetDeviceContext(), buffertype);
	auto buffer = projToWorld->GetBuffer();
	Graphic->GetDeviceContext()->PSSetConstantBuffers(1, 1, &buffer);
	
	
	LightMgr->Render();
	
	deferredBuffer->EndMRT();

}

void Renderer::RenderBlend()
{
	deferredBuffer->BeginMRT(L"Blend");


	// 모든 2D 렌더링을 시작하려면 Z 버퍼를 끕니다.

	Graphic->TurnZBufferOff();

	blendShader->Render();

	auto diffuse = deferredBuffer->GetShaderResourceView(DeferredBuffer::DIFFUSEMAP);
	auto shade = deferredBuffer->GetShaderResourceView(DeferredBuffer::SHADE);
	auto specular = deferredBuffer->GetShaderResourceView(DeferredBuffer::SPECULAR);


	Graphic->GetDeviceContext()->PSSetShaderResources(0, 1, &diffuse);
	Graphic->GetDeviceContext()->PSSetShaderResources(1, 1, &shade);
	Graphic->GetDeviceContext()->PSSetShaderResources(2, 1, &specular);



	
	rectangleBuffer->Render();


	deferredBuffer->Render();



	deferredBuffer->EndMRT();

	// 모든 2D 렌더링이 완료되었으므로 Z 버퍼를 다시 켜십시오.
	Graphic->TurnZBufferOn();

	//ImGui::CreateContext();

}

HRESULT Renderer::AddRenderGroup(RENDERID ID, shared_ptr<GameObject> _gameObject)
{
	if (ID >= RENDER_END || nullptr == _gameObject)
		return E_FAIL;

	RenderGroup[ID].push_back(_gameObject);

	return S_OK;
}
