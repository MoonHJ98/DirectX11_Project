#include "pch.h"
#include "RenderTargetManager.h"
#include "RenderTarget.h"
#include "GraphicDevice.h"

HRESULT RenderTargetManager::Initialize()
{
	Graphic = GraphicDevice::GetInstance();

	UINT viewportNum = 1;
	D3D11_VIEWPORT viewport;
	Graphic->GetDeviceContext()->RSGetViewports(&viewportNum, &viewport);


	// 깊이 버퍼의 구조체를 초기화합니다.
	D3D11_TEXTURE2D_DESC depthBufferDesc;
	ZeroMemory(&depthBufferDesc, sizeof(depthBufferDesc));

	// 깊이 버퍼의 구조체를 설정합니다.
	depthBufferDesc.Width = (int)viewport.Width;
	depthBufferDesc.Height = (int)viewport.Height;
	depthBufferDesc.MipLevels = 1;
	depthBufferDesc.ArraySize = 1;
	depthBufferDesc.Format = DXGI_FORMAT_D24_UNORM_S8_UINT;
	depthBufferDesc.SampleDesc.Count = 1;
	depthBufferDesc.SampleDesc.Quality = 0;
	depthBufferDesc.Usage = D3D11_USAGE_DEFAULT;
	depthBufferDesc.BindFlags = D3D11_BIND_DEPTH_STENCIL;
	depthBufferDesc.CPUAccessFlags = 0;
	depthBufferDesc.MiscFlags = 0;

	// 채워진 구조체를 사용하여 깊이 버퍼의 텍스처를 만듭니다.
	HRESULT hr = Graphic->GetDevice()->CreateTexture2D(&depthBufferDesc, NULL, depthStencilBuffer.GetAddressOf());
	if (FAILED(hr))
	{
		MSG_BOX("Failed to create DeferredBuffer's depth buffer.");
		return E_FAIL;
	}

	// 깊이 스텐실 뷰 구조체를 초기화합니다.
	D3D11_DEPTH_STENCIL_VIEW_DESC depthStencilViewDesc;
	ZeroMemory(&depthStencilViewDesc, sizeof(depthStencilViewDesc));

	// 깊이 스텐실 뷰 구조체를 설정합니다.
	depthStencilViewDesc.Format = DXGI_FORMAT_D24_UNORM_S8_UINT;
	depthStencilViewDesc.ViewDimension = D3D11_DSV_DIMENSION_TEXTURE2D;
	depthStencilViewDesc.Texture2D.MipSlice = 0;

	// 깊이 스텐실 뷰를 만듭니다.
	hr = Graphic->GetDevice()->CreateDepthStencilView(depthStencilBuffer.Get(), &depthStencilViewDesc, depthStencilView.GetAddressOf());
	if (FAILED(hr))
	{
		MSG_BOX("Failed to create DeferredBuffer's depth stencil view.");
		return E_FAIL;
	}

	// 렌더링을 위해 뷰포트를 설정합니다.
	//viewport.Width = (float)textureWidth;
	//viewport.Height = (float)textureHeight;
	//viewport.MinDepth = 0.0f;
	//viewport.MaxDepth = 1.0f;
	//viewport.TopLeftX = 0.0f;
	//viewport.TopLeftY = 0.0f;

	return S_OK;
}

HRESULT RenderTargetManager::AddRenderTarget(wstring _TargetTag, Vector3 _position, Vector3 _bitmapScale, Vector4 _clearColor)
{
	if (nullptr != FindRenderTarget(_TargetTag))
		return E_FAIL;

	auto renderTarget = RenderTarget::Create(_position, _bitmapScale, _clearColor);

	if (nullptr == renderTarget)
		return E_FAIL;

	renderTargets.insert(TARGET::value_type(_TargetTag, renderTarget));

	return S_OK;
}

HRESULT RenderTargetManager::AddMultiRenderTarget(wstring MRTTag, wstring TargetTag)
{

	 auto target = FindRenderTarget(TargetTag);

	if (nullptr == target)
		return E_FAIL;

	auto MRTList = FindMRT(MRTTag);

	if (nullptr == MRTList)
	{
		list<shared_ptr<RenderTarget>> MRTList;

		MRTList.push_back(target);

		multiRenderTargets.insert(MRT::value_type(MRTTag, MRTList));
	}
	else
		MRTList->push_back(target);


	return S_OK;
}

shared_ptr<RenderTarget> RenderTargetManager::FindRenderTarget(wstring _TargetTag)
{
	auto iter = renderTargets.find(_TargetTag);

	if (iter == renderTargets.end())
		return nullptr;

	return iter->second;
}

list<shared_ptr<RenderTarget>>* RenderTargetManager::FindMRT(wstring MRTTag)
{
	auto iter = multiRenderTargets.find(MRTTag);


	if (iter == multiRenderTargets.end())
		return nullptr;

	return &iter->second;
}

void RenderTargetManager::BeginMRT(wstring MRTTag)
{
	auto MRTList = FindMRT(MRTTag);

	if (nullptr == MRTList)
		return;

	for (auto& Target : *MRTList)
	{
		Target->BeginRender(depthStencilView.Get());
	}

}

void RenderTargetManager::EndMRT(wstring MRTTag)
{
	auto MRTList = FindMRT(MRTTag);

	if (nullptr == MRTList)
		return;

	for (auto& Target : *MRTList)
	{
		Target->EndRender();
	}
}

void RenderTargetManager::RenderTargets()
{
	for (auto iter : renderTargets)
	{
		iter.second->RenderBuffer();
	}
}
