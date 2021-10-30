#pragma once


#include "Includes.h"

class GraphicDevice;
class RectangleBuffer;
class RenderTarget;

class DeferredBuffer
{
public:
	enum BUFFER { DIFFUSE, NORMAL, BUFFER_END };

private:
	DeferredBuffer();
	DeferredBuffer(const DeferredBuffer& Rhs);

public:
	~DeferredBuffer();

private:
	HRESULT Initialize(int _textureWidth, int _textureHeight, float _screenDepth, float _screenNear);

public:
	ID3D11ShaderResourceView* GetShaderResourceView(int _view);
	void ClearRenderTargets(wstring MRTTag, float _red, float _green, float _blue, float _alpha);
	void SetRenderTargets(wstring MRTTag);

public:
	void Render();
	void BeginMRT(wstring MRTTag);
	void EndMRT();

	void AddMultiRenderTarget(wstring MRTTag, BUFFER id);
	
	vector<ID3D11RenderTargetView*>* FindMultiRenderTarget(wstring MRTTag);

public:
	static shared_ptr<DeferredBuffer> Create(int _textureWidth, int _textureHeight, float _screenDepth, float _screenNear);

private:
	shared_ptr<GraphicDevice> Graphic = nullptr;
	int textureWidth = 0;
	int textureHeight = 0;
	ComPtr<ID3D11Texture2D> renderTargetTextureArray[BUFFER_END] = {};
	ComPtr<ID3D11RenderTargetView> renderTargetViewArray[BUFFER_END] = {};
	ComPtr<ID3D11ShaderResourceView> shaderResourceViewArray[BUFFER_END] = {};
	ComPtr<ID3D11Texture2D> depthStencilBuffer = nullptr;       // 이건 렌더타켓 매니저에서 갖고있기.
	ComPtr<ID3D11DepthStencilView> depthStencilView = nullptr;
	D3D11_VIEWPORT viewport;

	shared_ptr<RectangleBuffer>         rectangleBuffer[BUFFER_END] = {};
	vector<shared_ptr<RenderTarget>> renderTargets;

	map<wstring, vector<ID3D11RenderTargetView*>> renderTargetMap;

};

