#pragma once

#include "Base.h"

class GraphicDevice;
class DebugWindow;
class Transform;
class Shader;


class RenderTarget : public Base
{
private:
	RenderTarget();
	RenderTarget(const RenderTarget& Rhs);

public:
	~RenderTarget();

public:
	void SetRenderTarget(ID3D11DepthStencilView* _DepthStencilView);
	void ClearRenderTarget(ID3D11DepthStencilView* _DepthStencilView, float _red, float _green, float _blue, float _alpha);
	ID3D11ShaderResourceView* GetShaderResourceView();

private:
	HRESULT Initialize(int screenWidth, int screenHeight, int bitmapWidth, int bitmapHeight);

public:
	virtual int Update(float _timeDelta) override;
	virtual void Render() override;
	void PostRender();

public:
	static shared_ptr<RenderTarget> Create(int screenWidth, int screenHeight, int bitmapWidth, int bitmapHeight);

private:
	shared_ptr<GraphicDevice>			Graphic = nullptr;
	ComPtr<ID3D11Texture2D>				RenderTargetTexture = nullptr;
	ComPtr<ID3D11RenderTargetView>		RenderTargetView = nullptr;
	ComPtr<ID3D11ShaderResourceView>	ShaderResourceView = nullptr;

	shared_ptr<DebugWindow>             debugWindow = nullptr;
	shared_ptr<Transform>				transform = nullptr;
	shared_ptr<Shader>					shader = nullptr;

};

