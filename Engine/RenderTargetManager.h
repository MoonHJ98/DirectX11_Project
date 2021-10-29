#pragma once

#include "Includes.h"

class RenderTarget;

class RenderTargetManager
{
	SINGLETON(RenderTargetManager)

private:
	typedef map<wstring, shared_ptr<RenderTarget>>			TARGET;
	typedef map<wstring, list<shared_ptr<RenderTarget>>>	MRT;

	HRESULT AddRenderTarget(wstring _TargetTag, Vector3 _position, int _screenWidth, int _sceenHeight, int _bitmapWidth, int _bitmapHeight, Vector4 ClearColor);

	shared_ptr<RenderTarget> FindRenderTarget(wstring _TargetTag);


private:
	TARGET targets;
	MRT Mrt;
};

