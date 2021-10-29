#pragma once

const int BUFFER_COUNT = 2;

#include "Includes.h"

class GraphicDevice;

class DefferedBuffer
{
private:
	DefferedBuffer();
	DefferedBuffer(const DefferedBuffer& Rhs);

public:
	~DefferedBuffer();

private:
	HRESULT Initialize(int _textureWidth, int _textureHeight, float _screenDepth, float _screenNear);

public:
	ID3D11ShaderResourceView* GetShaderResourceView(int _view);
	void ClearRenderTargets(float _red, float _green, float _blue, float _alpha);
	void SetRenderTargets();

public:
	static shared_ptr<DefferedBuffer> Create(int _textureWidth, int _textureHeight, float _screenDepth, float _screenNear);

private:
	shared_ptr<GraphicDevice> Graphic = nullptr;
	int textureWidth = 0;
	int textureHeight = 0;
	ComPtr<ID3D11Texture2D> renderTargetTextureArray[BUFFER_COUNT] = {};
	ComPtr<ID3D11RenderTargetView> renderTargetViewArray[BUFFER_COUNT] = {};
	ComPtr<ID3D11ShaderResourceView> shaderResourceViewArray[BUFFER_COUNT] = {};
	ComPtr<ID3D11Texture2D> depthStencilBuffer = nullptr;
	ComPtr<ID3D11DepthStencilView> depthStencilView = nullptr;
	D3D11_VIEWPORT viewport;
};

