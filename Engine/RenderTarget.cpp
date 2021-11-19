#include "pch.h"
#include "RenderTarget.h"
#include "GraphicDevice.h"
#include "Transform.h"
#include "RectangleBuffer.h"
#include "Shader.h"

RenderTarget::RenderTarget()
{
}

RenderTarget::RenderTarget(const RenderTarget & Rhs)
{
}

RenderTarget::~RenderTarget()
{
}

HRESULT RenderTarget::Initialize(Vector3 _position, Vector3 bitmapScale, Vector4 _ClearColor)
{
	ClearColor = _ClearColor;

	Graphic = GraphicDevice::GetInstance();

	rectangleBuffer = RectangleBuffer::Create();

	transform = Transform::Create(Transform::TRANSDESC());

	transform->SetState(Transform::POSITION, _position);
	transform->SetScale(bitmapScale);

	D3D11_INPUT_ELEMENT_DESC InputLayout[] =
	{
		{ "POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 0, D3D11_INPUT_PER_VERTEX_DATA, 0 },
		{ "TEXCOORD", 0, DXGI_FORMAT_R32G32_FLOAT, 0, D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_VERTEX_DATA, 0 },

	};

	shader = Shader::Create(InputLayout, sizeof(InputLayout), L"../Engine/TextureVS.hlsl", L"../Engine/TexturePS.hlsl");


	return S_OK;
}

void RenderTarget::Render(ID3D11ShaderResourceView* _ShaderResourceView)
{

	shader->Render();

	transform->Update();

	Graphic->GetDeviceContext()->PSSetShaderResources(0, 1, &_ShaderResourceView);

	rectangleBuffer->Render();
}

shared_ptr<RenderTarget> RenderTarget::Create(Vector3 _position, Vector3 bitmapScale, Vector4 _ClearColor)
{
	shared_ptr<RenderTarget> Instance(new RenderTarget());
	if (FAILED(Instance->Initialize(_position, bitmapScale, _ClearColor)))
	{
		MSG_BOX("Failed to create RenderTarget.");
		return nullptr;
	}
	return Instance;
}
