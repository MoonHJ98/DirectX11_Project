#include "pch.h"
#include "Renderer.h"
#include "GraphicDevice.h"
#include "GameObject.h"
#include "DeferredBuffer.h"
#include "RenderTargetManager.h"
#include "LightManager.h"
#include "Shader.h"


HRESULT Renderer::Initialize()
{
	Graphic = GraphicDevice::GetInstance();

	UINT viewportNum = 1;
	D3D11_VIEWPORT viewport;
	Graphic->GetDeviceContext()->RSGetViewports(&viewportNum, &viewport);

	LightMgr = LightManager::GetInstance();

	deferredBuffer = DeferredBuffer::Create((int)viewport.Width, (int)viewport.Height, SCREENDEPTH, SCREENNEAR);

	deferredBuffer->AddMultiRenderTarget(L"Deferred", DeferredBuffer::DIFFUSE);
	deferredBuffer->AddMultiRenderTarget(L"Deferred", DeferredBuffer::NORMAL);




	lightShader = Shader::Create(nullptr, 0, L"", L"../Engine/LightAccPS.hlsl");

	return S_OK;
}

void Renderer::Render()
{
	RenderPriority();

	RenderNonAlpha();

	RenderAlpha();

	RenderUI();

	RenderLightAcc();

	RenderBlend();
}

void Renderer::RenderPriority()
{
}

void Renderer::RenderNonAlpha()
{
	deferredBuffer->BeginMRT(L"Deferred");
	//renderTargetMGR->BeginMRT(L"Deferred");
	for (auto& pGameObject : RenderGroup[RENDER_NONALPHA])
	{
		if (nullptr != pGameObject)
		{
			pGameObject->Render();
		}
	}
	//renderTargetMGR->EndMRT(L"Deferred");
	deferredBuffer->EndMRT();
	RenderGroup[RENDER_NONALPHA].clear();

}

void Renderer::RenderAlpha()
{
}

void Renderer::RenderUI()
{
	// ��� 2D �������� �����Ϸ��� Z ���۸� ���ϴ�.
	Graphic->TurnZBufferOff();

	//renderTargetMGR->RenderTargets();
	deferredBuffer->Render();

	// ��� 2D �������� �Ϸ�Ǿ����Ƿ� Z ���۸� �ٽ� �ѽʽÿ�.
	Graphic->TurnZBufferOn();
}

void Renderer::RenderLightAcc()
{
	//lightShader->Render();



}

void Renderer::RenderBlend()
{
}

HRESULT Renderer::AddRenderGroup(RENDERID ID, shared_ptr<GameObject> _gameObject)
{
	if (ID >= RENDER_END || nullptr == _gameObject)
		return E_FAIL;

	RenderGroup[ID].push_back(_gameObject);

	return S_OK;
}
