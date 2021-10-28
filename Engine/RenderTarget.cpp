#include "pch.h"
#include "RenderTarget.h"
#include "GraphicDevice.h"
#include "DebugWindow.h"
#include "Transform.h"
#include "Shader.h"
#include "Management.h"
#include "RectangleBuffer.h"

RenderTarget::RenderTarget()
{
}

RenderTarget::RenderTarget(const RenderTarget & Rhs)
{
}

RenderTarget::~RenderTarget()
{
}

void RenderTarget::SetRenderTarget(ID3D11DepthStencilView* _DepthStencilView)
{
	// 렌더링 대상 뷰와 깊이 스텐실 버퍼를 출력 렌터 파이프 라인에 바인딩한다.
	Graphic->GetDeviceContext()->OMSetRenderTargets(1, RenderTargetView.GetAddressOf(), _DepthStencilView);
}

void RenderTarget::ClearRenderTarget(ID3D11DepthStencilView* _DepthStencilView, float _red, float _green, float _blue, float _alpha)
{
	// 버퍼를 지울 색을 설정.
	float color[4] = { _red, _green, _blue, _alpha };

	// 백버퍼를 지운다.
	Graphic->GetDeviceContext()->ClearRenderTargetView(RenderTargetView.Get(), color);

	// 깊이 버퍼를 지운다.
	Graphic->GetDeviceContext()->ClearDepthStencilView(_DepthStencilView, D3D11_CLEAR_DEPTH, 1.f, 0);
}

ID3D11ShaderResourceView * RenderTarget::GetShaderResourceView()
{
	return ShaderResourceView.Get();
}

HRESULT RenderTarget::Initialize(shared_ptr<Camera> _camera, int _positionX, int _positionY, int screenWidth, int screenHeight, int bitmapWidth, int bitmapHeight)
{
	camera = _camera;
	Graphic = GraphicDevice::GetInstance();

	//debugWindow = DebugWindow::Create(50, 50, screenWidth, screenHeight, bitmapWidth, bitmapHeight);
	rectangleBuffer = RectangleBuffer::Create();

	transform = Transform::Create(Transform::TRANSDESC());

	//transform->SetState(Transform::POSITION, Vector3((float)_positionX, (float)_positionY, 0.f));


	D3D11_INPUT_ELEMENT_DESC InputLayout[] =
	{
		{ "POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 0, D3D11_INPUT_PER_VERTEX_DATA, 0 },
		{ "TEXCOORD", 0, DXGI_FORMAT_R32G32_FLOAT, 0, D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_VERTEX_DATA, 0 },
		
	};

	shader = Shader::Create(InputLayout, sizeof(InputLayout), L"../Engine/TextureVS.hlsl", L"../Engine/TexturePS.hlsl");

	D3D11_TEXTURE2D_DESC textureDesc;
	ZeroMemory(&textureDesc, sizeof(textureDesc));

	// 렌더 타켓 텍스쳐 설정.
	textureDesc.Width = screenWidth;
	textureDesc.Height = screenHeight;
	textureDesc.MipLevels = 1;
	textureDesc.ArraySize = 1;
	textureDesc.Format = DXGI_FORMAT_R32G32B32A32_FLOAT;
	textureDesc.SampleDesc.Count = 1;
	textureDesc.Usage = D3D11_USAGE_DEFAULT;
	textureDesc.BindFlags = D3D11_BIND_RENDER_TARGET | D3D11_BIND_SHADER_RESOURCE;
	textureDesc.CPUAccessFlags = 0;
	textureDesc.MiscFlags = 0;

	// 렌더 타켓 텍스쳐 생성.
	if (FAILED(Graphic->GetDevice()->CreateTexture2D(&textureDesc, nullptr, RenderTargetTexture.GetAddressOf())))
	{
		MSG_BOX("Failed to create RenderTarget texture.");
		return E_FAIL;
	}

	// 렌더 타켓 뷰의 설명 설정
	D3D11_RENDER_TARGET_VIEW_DESC renderTargetViewDesc;
	renderTargetViewDesc.Format = textureDesc.Format;
	renderTargetViewDesc.ViewDimension = D3D11_RTV_DIMENSION_TEXTURE2D;
	renderTargetViewDesc.Texture2D.MipSlice = 0;

	// 렌더 타켓 뷰 생성.
	if (FAILED(Graphic->GetDevice()->CreateRenderTargetView(RenderTargetTexture.Get(), &renderTargetViewDesc, RenderTargetView.GetAddressOf())))
	{
		MSG_BOX("Failed to create RenderTarget View.");
		return E_FAIL;
	}

	// 셰이더 리소스 뷰 설정.
	D3D11_SHADER_RESOURCE_VIEW_DESC shaderResourceViewDesc;
	shaderResourceViewDesc.Format = textureDesc.Format;
	shaderResourceViewDesc.ViewDimension = D3D11_SRV_DIMENSION_TEXTURE2D;
	shaderResourceViewDesc.Texture2D.MostDetailedMip = 0;
	shaderResourceViewDesc.Texture2D.MipLevels = 1;

	// 셰이더 리소스 뷰 생성.
	if (FAILED(Graphic->GetDevice()->CreateShaderResourceView(RenderTargetTexture.Get(), &shaderResourceViewDesc, ShaderResourceView.GetAddressOf())))
	{
		MSG_BOX("Faied to create Shader Resource View.");
		return E_FAIL;
	}

	return S_OK;
}

shared_ptr<RenderTarget> RenderTarget::Create(shared_ptr<Camera> _camera, int _positionX, int _positionY, int screenWidth, int screenHeight, int bitmapWidth, int bitmapHeight)
{
	shared_ptr<RenderTarget> Instance(new RenderTarget());
	if (FAILED(Instance->Initialize(_camera, _positionX, _positionY, screenWidth, screenHeight, bitmapWidth, bitmapHeight)))
	{
		MSG_BOX("Failed to create RenderTarget.");
		return nullptr;
	}
	return Instance;
}

int RenderTarget::Update(float _timeDelta)
{

	return 0;
}

void RenderTarget::Render()
{
	// 렌더링 대상을 렌더링에 맞게 설정합니다.
	SetRenderTarget(Graphic->GetDepthStencilView());

	// 렌더링을 텍스처에 지웁니다.
	ClearRenderTarget(Graphic->GetDepthStencilView(), 1.0f, 1.0f, 1.0f, 1.0f);

	// 이제 장면을 렌더링하면 백 버퍼 대신 텍스처로 렌더링됩니다.
	Management::GetInstance()->Render();

	// 렌더링 대상을 원래의 백 버퍼로 다시 설정하고 렌더링에 대한 렌더링을 더 이상 다시 설정하지 않습니다.
	Graphic->SetBackBufferRenderTarget();
}

void RenderTarget::PostRender()
{
	transform->Update(true, false, camera.lock());

	rectangleBuffer->Render();
	//debugWindow->Render();
	shader->Render();

	Graphic->GetDeviceContext()->PSSetShaderResources(0, 1, ShaderResourceView.GetAddressOf());

	
	Graphic->GetDeviceContext()->DrawIndexed(/*debugWindow*/rectangleBuffer->GetIndexCount(), 0, 0);


}
