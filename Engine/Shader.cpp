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

HRESULT Shader::Initialize(D3D11_INPUT_ELEMENT_DESC InputlayoutDesc[], UINT layoutSize, wstring _VSPath, wstring _PSPath)
{
	componentName = L"Shader";
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
	if (L"" != _VSPath)
	{
		if (FAILED(D3DCompileFromFile(_VSPath.c_str(), NULL, NULL, "main", "vs_4_0", ShaderFlag, 0, &vertexShaderBuffer, NULL)))
		{
			MSG_BOX("Failed to compile vertex shader.");
			return E_FAIL;
		}
		// Create the vertex shader from the buffer.
		if (FAILED(Graphic->GetDevice()->CreateVertexShader(vertexShaderBuffer->GetBufferPointer(), vertexShaderBuffer->GetBufferSize(), NULL, VertexShader.GetAddressOf())))
		{
			MSG_BOX("Failed to create vertex shader from buffer.");
			return E_FAIL;
		}

		numElements = layoutSize / sizeof(D3D11_INPUT_ELEMENT_DESC);

		// Create the vertex input layout.
		if (FAILED(Graphic->GetDevice()->CreateInputLayout(InputlayoutDesc, numElements, vertexShaderBuffer->GetBufferPointer(), vertexShaderBuffer->GetBufferSize(), Layout.GetAddressOf())))
		{
			MSG_BOX("Failed to create vertex input layout.");
			return E_FAIL;
		}
	}


	if (L"" != _PSPath)
	{
		// Compile the pixel shader code.
		if (FAILED(D3DCompileFromFile(_PSPath.c_str(), NULL, NULL, "main", "ps_4_0", ShaderFlag, 0, &pixelShaderBuffer, NULL)))
		{
			MSG_BOX("Failed to compile pixel shader.");
			return E_FAIL;
		}



		// Create the pixel shader from the buffer.
		if (FAILED(Graphic->GetDevice()->CreatePixelShader(pixelShaderBuffer->GetBufferPointer(), pixelShaderBuffer->GetBufferSize(), NULL, PixelShader.GetAddressOf())))
		{
			MSG_BOX("Failed to create pixel shader from buffer.");
			return E_FAIL;
		}
	}


	// Release the vertex shader buffer and pixel shader buffer since they are no longer needed.
	if (nullptr != vertexShaderBuffer)
	{
		vertexShaderBuffer->Release();
		vertexShaderBuffer = nullptr;
	}

	if (nullptr != PixelShader)
	{
		pixelShaderBuffer->Release();
		pixelShaderBuffer = nullptr;
	}

	// 텍스처 샘플러 생태 구조체 생성 및 설정
	D3D11_SAMPLER_DESC samplerDecs;
	samplerDecs.Filter = D3D11_FILTER_MIN_MAG_MIP_LINEAR; // 텍스쳐를 어떻게 처리 할 것인지
	samplerDecs.AddressU = D3D11_TEXTURE_ADDRESS_WRAP;	// UV좌표 0~1 사이에 모든 이미지가 오게 설정
	samplerDecs.AddressV = D3D11_TEXTURE_ADDRESS_WRAP;	// UV좌표 0~1 사이에 모든 이미지가 오게 설정
	samplerDecs.AddressW = D3D11_TEXTURE_ADDRESS_WRAP;	// UV좌표 0~1 사이에 모든 이미지가 오게 설정
	samplerDecs.MipLODBias = 0.f;                         // 아래부터 그냥 Default
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
	if(nullptr != Layout)
		Graphic->GetDeviceContext()->IASetInputLayout(Layout.Get());

	if(nullptr != VertexShader)
		Graphic->GetDeviceContext()->VSSetShader(VertexShader.Get(), NULL, 0);

	if(nullptr != PixelShader)
		Graphic->GetDeviceContext()->PSSetShader(PixelShader.Get(), NULL, 0);

	// 픽셀쉐이더에 있는 sampler state 설정
	if(nullptr != SampleState)
		Graphic->GetDeviceContext()->PSSetSamplers(0, 1, &SampleState);


}

void Shader::RenderInspector()
{


}

shared_ptr<Shader> Shader::Create(D3D11_INPUT_ELEMENT_DESC InputlayoutDesc[], UINT layoutSize, wstring _VSPath, wstring _PSPath)
{
	shared_ptr<Shader> Instance(new Shader());

	if (FAILED(Instance->Initialize(InputlayoutDesc, layoutSize, _VSPath, _PSPath)))
	{
		MSG_BOX("Failed to create Shader.");
		return nullptr;
	}

	return Instance;
}

int Shader::Update(float _timeDelta)
{
	return 0;
}
