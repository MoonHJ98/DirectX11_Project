#include "pch.h"
#include "Shader.h"
#include "GraphicDevice.h"

Shader::Shader()
{
}

Shader::Shader(const Shader & Rhs)
{
}

Shader::~Shader()
{
}

HRESULT Shader::Initialize(wstring _VSPath, wstring _PSPath)
{
	Graphic = GraphicDevice::GetInstance();
	ID3D10Blob* vertexShaderBuffer;
	ID3D10Blob* pixelShaderBuffer;

	unsigned int numElements;
	DWORD ShaderFlag = D3DCOMPILE_ENABLE_STRICTNESS | D3DCOMPILE_DEBUG | D3DCOMPILE_SKIP_OPTIMIZATION;

	// Initialize the pointers this function will use to null.
	vertexShaderBuffer = nullptr;
	pixelShaderBuffer = nullptr;

	// Initialize the pointers this function will use to null.
	vertexShaderBuffer = nullptr;
	pixelShaderBuffer = nullptr;

	// Compile the vertex shader code.
	if (FAILED(D3DCompileFromFile(_VSPath.c_str(), NULL, NULL, "main", "vs_4_0", ShaderFlag, 0, &vertexShaderBuffer, NULL)))
	{
		MSG_BOX("Failed to compile vertex shader.");
		return E_FAIL;
	}

	// Compile the pixel shader code.
	if (FAILED(D3DCompileFromFile(_PSPath.c_str(), NULL, NULL, "main", "ps_4_0", D3D10_SHADER_ENABLE_STRICTNESS, 0, &pixelShaderBuffer, NULL)))
	{
		MSG_BOX("Failed to compile pixel shader.");
		return E_FAIL;
	}

	// Create the vertex shader from the buffer.
	if (FAILED(Graphic->GetDevice()->CreateVertexShader(vertexShaderBuffer->GetBufferPointer(), vertexShaderBuffer->GetBufferSize(), NULL, VertexShader.GetAddressOf())))
	{
		MSG_BOX("Failed to create vertex shader from buffer.");
		return E_FAIL;
	}

	// Create the pixel shader from the buffer.
	if (FAILED(Graphic->GetDevice()->CreatePixelShader(pixelShaderBuffer->GetBufferPointer(), pixelShaderBuffer->GetBufferSize(), NULL, PixelShader.GetAddressOf())))
	{
		MSG_BOX("Failed to create pixel shader from buffer.");
		return E_FAIL;
	}

	// Create the vertex input layout description.
	D3D11_INPUT_ELEMENT_DESC polygonLayout[] =
	{
		{ "POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 0, D3D11_INPUT_PER_VERTEX_DATA, 0 },
		{ "TEXCOORD", 0, DXGI_FORMAT_R32G32_FLOAT, 0, D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_VERTEX_DATA, 0 },
		{ "NORMAL", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_VERTEX_DATA, 0 },
		{ "TANGENT", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_VERTEX_DATA, 0 },
		{ "BLENDWEIGHTS", 0, DXGI_FORMAT_R32G32B32A32_FLOAT, 0, D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_VERTEX_DATA, 0 },
		{ "BLENDINDICES", 0, DXGI_FORMAT_R32G32B32A32_FLOAT, 0, D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_VERTEX_DATA, 0 }
	};

	numElements = ARRAYSIZE(polygonLayout);

	// Create the vertex input layout.
	if (FAILED(Graphic->GetDevice()->CreateInputLayout(polygonLayout, numElements, vertexShaderBuffer->GetBufferPointer(), vertexShaderBuffer->GetBufferSize(), Layout.GetAddressOf())))
	{
		MSG_BOX("Failed to create vertex input layout.");
		return E_FAIL;
	}

	// Release the vertex shader buffer and pixel shader buffer since they are no longer needed.
	vertexShaderBuffer->Release();
	vertexShaderBuffer = nullptr;

	pixelShaderBuffer->Release();
	pixelShaderBuffer = nullptr;

	// �ؽ�ó ���÷� ���� ����ü ���� �� ����
	D3D11_SAMPLER_DESC samplerDecs;
	samplerDecs.Filter = D3D11_FILTER_MIN_MAG_MIP_LINEAR; // �ؽ��ĸ� ��� ó�� �� ������
	samplerDecs.AddressU = D3D11_TEXTURE_ADDRESS_WRAP;	// UV��ǥ 0~1 ���̿� ��� �̹����� ���� ����
	samplerDecs.AddressV = D3D11_TEXTURE_ADDRESS_WRAP;	// UV��ǥ 0~1 ���̿� ��� �̹����� ���� ����
	samplerDecs.AddressW = D3D11_TEXTURE_ADDRESS_WRAP;	// UV��ǥ 0~1 ���̿� ��� �̹����� ���� ����
	samplerDecs.MipLODBias = 0.f;                         // �Ʒ����� �׳� Default
	samplerDecs.MaxAnisotropy = 1;
	samplerDecs.ComparisonFunc = D3D11_COMPARISON_ALWAYS;
	samplerDecs.BorderColor[0] = 0;
	samplerDecs.BorderColor[1] = 0;
	samplerDecs.BorderColor[2] = 0;
	samplerDecs.BorderColor[3] = 0;
	samplerDecs.MinLOD = 0;
	samplerDecs.MaxLOD = D3D11_FLOAT32_MAX;

	if (FAILED(Graphic->GetDevice()->CreateSamplerState(&samplerDecs, SampleState.GetAddressOf())))
	{
		MSG_BOX("Failed to create SamplerState.");
		return E_FAIL;
	}
	return S_OK;
}

void Shader::Render()
{
	Graphic->GetDeviceContext()->IASetInputLayout(Layout.Get());

	Graphic->GetDeviceContext()->VSSetShader(VertexShader.Get(), NULL, 0);
	Graphic->GetDeviceContext()->PSSetShader(PixelShader.Get(), NULL, 0);

	// �ȼ����̴��� �ִ� sampler state ����
	Graphic->GetDeviceContext()->PSSetSamplers(0, 1, &SampleState);


}

shared_ptr<Shader> Shader::Create(wstring _VSPath, wstring _PSPath)
{
	shared_ptr<Shader> Instance(new Shader());

	if (FAILED(Instance->Initialize(_VSPath, _PSPath)))
	{
		MSG_BOX("Failed to create Shader.");
		return nullptr;
	}

	return Instance;
}
