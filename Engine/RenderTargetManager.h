#pragma once

#include "Includes.h"

class RenderTarget;
class GraphicDevice;

class RenderTargetManager
{
	SINGLETON(RenderTargetManager)

private:
	typedef map<wstring, shared_ptr<RenderTarget>>			TARGET;
	typedef map<wstring, list<shared_ptr<RenderTarget>>>	MRT;

public:
	HRESULT Initialize();

public:
	HRESULT AddRenderTarget(wstring _TargetTag, Vector3 _position, Vector3 _bitmapScale, Vector4 _clearColor);
	HRESULT	AddMultiRenderTarget(wstring MRTTag, wstring TargetTag);

	shared_ptr<RenderTarget> FindRenderTarget(wstring _TargetTag);
	list<shared_ptr<RenderTarget>>*	 FindMRT(wstring MRTTag);

	void	 BeginMRT(wstring MRTTag);
	void	 EndMRT(wstring MRTTag);

	void     RenderTargets();

private:
	shared_ptr<GraphicDevice> Graphic = nullptr;

	TARGET renderTargets;
	MRT multiRenderTargets;

	ComPtr<ID3D11Texture2D> depthStencilBuffer = nullptr;       // 이건 렌더타켓 매니저에서 갖고있기.
	ComPtr<ID3D11DepthStencilView> depthStencilView = nullptr;
};

