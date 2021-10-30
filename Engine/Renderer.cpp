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
	// 모든 2D 렌더링을 시작하려면 Z 버퍼를 끕니다.
	Graphic->TurnZBufferOff();

	//renderTargetMGR->RenderTargets();
	deferredBuffer->Render();

	// 모든 2D 렌더링이 완료되었으므로 Z 버퍼를 다시 켜십시오.
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
