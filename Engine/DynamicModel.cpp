#include "pch.h"
#include "DynamicModel.h"
#include "GraphicDevice.h"
#include "AssimpLoader.h"
#include "AnimationController.h"
#include "Management.h"
#include "Texture.h"
#include <winnt.h>

DynamicModel::~DynamicModel()
{
	
}

HRESULT DynamicModel::Initialize(ID3D11Device * _Device, string _MeshFilePath, wstring VSPath, wstring _PSPath)
{
	if (FAILED(LoadModel(_MeshFilePath)))
		return E_FAIL;
	
	if (FAILED(InitializeShader(_Device, VSPath, _PSPath)))
		return E_FAIL;

	// Constant Buffer.
	MatrixBuffer.Create(_Device);
	BoneTransformBuffer.Create(_Device);

	return S_OK;
}

HRESULT DynamicModel::LoadModel(string _MeshFilePath)
{
	return AssimpLoader::GetInstance()->LoadModel(_MeshFilePath, Meshes, Textures, &Animator);
}

HRESULT DynamicModel::InitializeShader(ID3D11Device * _Device, wstring _VSPath, wstring _PSPath)
{
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
	if (FAILED(D3DCompileFromFile(_VSPath.c_str(), NULL, NULL, "main", "vs_5_0", ShaderFlag, 0, &vertexShaderBuffer, NULL)))
	{
		MSG_BOX("Failed to compile vertex shader.");
		return E_FAIL;
	}

	// Compile the pixel shader code.
	if (FAILED(D3DCompileFromFile(_PSPath.c_str(), NULL, NULL, "main", "ps_5_0", D3D10_SHADER_ENABLE_STRICTNESS, 0, &pixelShaderBuffer, NULL)))
	{
		MSG_BOX("Failed to compile pixel shader.");
		return E_FAIL;
	}

	// Create the vertex shader from the buffer.
	if (FAILED(_Device->CreateVertexShader(vertexShaderBuffer->GetBufferPointer(), vertexShaderBuffer->GetBufferSize(), NULL, VertexShader.GetAddressOf())))
	{
		MSG_BOX("Failed to create vertex shader from buffer.");
		return E_FAIL;
	}
	
	// Create the pixel shader from the buffer.
	if (FAILED(_Device->CreatePixelShader(pixelShaderBuffer->GetBufferPointer(), pixelShaderBuffer->GetBufferSize(), NULL, PixelShader.GetAddressOf())))
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
		{ "WEIGHT", 0, DXGI_FORMAT_R32G32B32A32_FLOAT, 0, D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_VERTEX_DATA, 0 },
		{ "BLENDINDICES", 0, DXGI_FORMAT_R32G32B32A32_UINT, 0, D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_VERTEX_DATA, 0 }
	};

	numElements = ARRAYSIZE(polygonLayout);

	// Create the vertex input layout.
	if (FAILED(_Device->CreateInputLayout(polygonLayout, numElements, vertexShaderBuffer->GetBufferPointer(), vertexShaderBuffer->GetBufferSize(), Layout.GetAddressOf())))
	{
		MSG_BOX("Failed to create vertex input layout.");
		return E_FAIL;
	}

	// Release the vertex shader buffer and pixel shader buffer since they are no longer needed.
	vertexShaderBuffer->Release();
	vertexShaderBuffer = nullptr;

	pixelShaderBuffer->Release();
	pixelShaderBuffer = nullptr;

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

	if (FAILED(_Device->CreateSamplerState(&samplerDecs, SampleState.GetAddressOf())))
	{
		MSG_BOX("Failed to create SamplerState.");
		return E_FAIL;
	}

	return S_OK;
}

void DynamicModel::Render(MATRIXBUFFERTYPE _MatrixbufferDesc, float _timeDelta)
{
	auto GraphicDev = GraphicDevice::GetInstance();
	MatrixBuffer.SetData(GraphicDev->GetDeviceContext(), _MatrixbufferDesc);
	auto buffer = MatrixBuffer.GetBuffer();
	GraphicDev->GetDeviceContext()->VSSetConstantBuffers(0, 1, &buffer);

	Animator->SetAnimation("Take 001");

	vector<XMFLOAT4X4> finalTransforms = Animator->GetTransforms(_timeDelta);

	BoneTransformBuffer.SetData(GraphicDev->GetDeviceContext(), finalTransforms);
	auto Transform = BoneTransformBuffer.GetBuffer();
	GraphicDev->GetDeviceContext()->VSSetConstantBuffers(1, 1, &Transform);


	GraphicDev->GetDeviceContext()->IASetInputLayout(Layout.Get());

	GraphicDev->GetDeviceContext()->VSSetShader(VertexShader.Get(), NULL, 0);
	GraphicDev->GetDeviceContext()->PSSetShader(PixelShader.Get(), NULL, 0);

	// 픽셀쉐이더에 있는 sampler state 설정
	GraphicDev->GetDeviceContext()->PSSetSamplers(0, 1, &SampleState);

	for (UINT i = 0; i < Meshes.size(); ++i)
	{
		if (Meshes[i]->Diffuse)
			GraphicDev->GetDeviceContext()->PSSetShaderResources(0, 1, Meshes[i]->Diffuse->GetTexture());
		Meshes[i]->RenderBuffer(GraphicDev->GetDeviceContext());
	}
}

shared_ptr<DynamicModel> DynamicModel::Create(ID3D11Device * _Device, string _MeshFilePath, wstring _VSPath, wstring _PSPath)
{
	shared_ptr<DynamicModel> Instance(new DynamicModel());

	if (FAILED(Instance->Initialize(_Device, _MeshFilePath, _VSPath, _PSPath)))
	{
		MSG_BOX("Failed to create DynamicModel.");
		return nullptr;
	}
	return Instance;
}
