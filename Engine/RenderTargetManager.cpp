#include "pch.h"
#include "RenderTargetManager.h"
#include "RenderTarget.h"

HRESULT RenderTargetManager::AddRenderTarget(wstring _TargetTag, Vector3 _position, int _screenWidth, int _sceenHeight, int _bitmapWidth, int _bitmapHeight, Vector4 ClearColor)
{
	if (nullptr != FindRenderTarget(_TargetTag))
		return E_FAIL;

	auto renderTarget = RenderTarget::Create(_position, _screenWidth, _sceenHeight, _bitmapWidth, _bitmapHeight);

	if (nullptr == renderTarget)
		return E_FAIL;

	targets.insert(TARGET::value_type(_TargetTag, renderTarget));

	return S_OK;
}

shared_ptr<RenderTarget> RenderTargetManager::FindRenderTarget(wstring _TargetTag)
{
	auto iter = targets.find(_TargetTag);

	if (iter == targets.end())
		return nullptr;

	return iter->second;
}
