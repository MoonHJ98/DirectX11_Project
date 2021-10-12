#pragma once

#include "Includes.h"

class Shader
{
private:
	Shader();
	Shader(const Shader& Rhs);

public:
	~Shader();

private:
	HRESULT Initialize(wstring _VSPath, wstring _PSPath);
	void    Render();

public:
	shared_ptr<Shader> Create(wstring _VSPath, wstring _PSPath);

private:
	ComPtr<ID3D11VertexShader>			VertexShader = nullptr;
	ComPtr<ID3D11PixelShader>			PixelShader = nullptr;
	ComPtr<ID3D11InputLayout>			Layout = nullptr;
	ComPtr<ID3D11SamplerState>			SampleState = nullptr;
	shared_ptr<GraphicDevice>			Graphic = nullptr;
};
