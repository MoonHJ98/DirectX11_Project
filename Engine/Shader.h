#pragma once

#include "Component.h"

class GraphicDevice;

class Shader : public Component, public enable_shared_from_this<Shader>
{
private:
	Shader();
	Shader(const Shader& Rhs);

public:
	~Shader();

private:
	HRESULT Initialize(D3D11_INPUT_ELEMENT_DESC InputlayoutDesc[], UINT layoutSize, wstring _VSPath, wstring _PSPath);

public:	
	virtual int Update(float _timeDelta) override;
	virtual void    Render() override;
	virtual void RenderInspector() override;

public:
	static shared_ptr<Shader> Create(D3D11_INPUT_ELEMENT_DESC InputlayoutDesc[], UINT layoutSize, wstring _VSPath, wstring _PSPath);

private:
	ComPtr<ID3D11VertexShader>			VertexShader = nullptr;
	ComPtr<ID3D11PixelShader>			PixelShader = nullptr;
	ComPtr<ID3D11InputLayout>			Layout = nullptr;
	ComPtr<ID3D11SamplerState>			SampleState = nullptr;
	shared_ptr<GraphicDevice>			Graphic = nullptr;


};

