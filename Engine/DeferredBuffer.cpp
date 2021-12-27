#include "pch.h"
#include "DeferredBuffer.h"
#include "GraphicDevice.h"
#include "Management.h"
#include "RenderTarget.h"


DeferredBuffer::DeferredBuffer()
{
}

DeferredBuffer::DeferredBuffer(const DeferredBuffer & Rhs)
{
}

DeferredBuffer::~DeferredBuffer()
{
}

HRESULT DeferredBuffer::Initialize(int _textureWidth, int _textureHeight, float _screenDepth, float _screenNear)
{
	auto Diffuse = RenderTarget::Create(Vector3(-350.f, 250.f, 0.1f), Vector3(50.f, 50.f, 1.f), Vector4(1.f, 1.f, 1.f, 1.f));
	renderTargets.push_back(Diffuse);

	auto Normal = RenderTarget::Create(Vector3(-250, 250, 0.1f), Vector3(50.f, 50.f, 1.f), Vector4(1.f, 1.f, 1.f, 1.f));
	renderTargets.push_back(Normal);

	auto SpecularMap = RenderTarget::Create(Vector3(-350.f, 150.f, 0.1f), Vector3(50.f, 50.f, 1.f), Vector4(1.f, 1.f, 1.f, 1.f));
	renderTargets.push_back(SpecularMap);

	auto ViewDirection = RenderTarget::Create(Vector3(-250, 150.f, 0.1f), Vector3(50.f, 50.f, 1.f), Vector4(1.f, 1.f, 1.f, 1.f));
	renderTargets.push_back(ViewDirection);

	auto shade = RenderTarget::Create(Vector3(-350.f, 50.f, 0.1f), Vector3(50.f, 50.f, 1.f), Vector4(1.f, 1.f, 1.f, 1.f));
	renderTargets.push_back(shade);

	auto specular = RenderTarget::Create(Vector3(-250, 50.f, 0.1f), Vector3(50.f, 50.f, 1.f), Vector4(1.f, 1.f, 1.f, 1.f));
	renderTargets.push_back(specular);

	auto depthForShadow = RenderTarget::Create(Vector3(-350.f, -50.f, 0.1f), Vector3(50.f, 50.f, 1.f), Vector4(1.f, 1.f, 1.f, 1.f));
	renderTargets.push_back(depthForShadow);

	auto lightViewPos = RenderTarget::Create(Vector3(-250, -50.f, 0.1f), Vector3(50.f, 50.f, 1.f), Vector4(1.f, 1.f, 1.f, 1.f));
	renderTargets.push_back(lightViewPos);

	auto blend = RenderTarget::Create(Vector3(-350.f, -150.f, 0.1f), Vector3(50.f, 50.f, 1.f), Vector4(1.f, 1.f, 1.f, 1.f));
	renderTargets.push_back(blend);

	

	Graphic = GraphicDevice::GetInstance();

	HRESULT hr;
	textureWidth = _textureWidth;
	textureHeight = _textureHeight;

	// 렌더 타겟 텍스처 구조체를 초기화합니다.
	D3D11_TEXTURE2D_DESC textureDesc;
	ZeroMemory(&textureDesc, sizeof(textureDesc));

	// 렌더 타겟 텍스처 구조체를 설정합니다.
	textureDesc.Width = textureWidth;
	textureDesc.Height = textureHeight;
	textureDesc.MipLevels = 1;
	textureDesc.ArraySize = 1;
	textureDesc.Format = DXGI_FORMAT_R32G32B32A32_FLOAT;
	textureDesc.SampleDesc.Count = 1;
	textureDesc.Usage = D3D11_USAGE_DEFAULT;
	textureDesc.BindFlags = D3D11_BIND_RENDER_TARGET | D3D11_BIND_SHADER_RESOURCE;
	textureDesc.CPUAccessFlags = 0;
	textureDesc.MiscFlags = 0;


	// 렌더 타겟 텍스처를 만듭니다.
	for (int i = 0; i < BUFFER_END; i++)
	{
		hr = Graphic->GetDevice()->CreateTexture2D(&textureDesc, NULL, renderTargetTextureArray[i].GetAddressOf());
		if (FAILED(hr))
		{
			MSG_BOX("Failed to craete DeferredBuffer's render target texture.");
			return E_FAIL;
		}
	}

	// 렌더 타겟 뷰의 구조체를 설정합니다.
	D3D11_RENDER_TARGET_VIEW_DESC renderTargetViewDesc;
	renderTargetViewDesc.Format = textureDesc.Format;
	renderTargetViewDesc.ViewDimension = D3D11_RTV_DIMENSION_TEXTURE2D;
	renderTargetViewDesc.Texture2D.MipSlice = 0;

	// 렌더 타겟 뷰를 생성합니다.
	for (int i = 0; i < BUFFER_END; i++)
	{
		hr = Graphic->GetDevice()->CreateRenderTargetView(renderTargetTextureArray[i].Get(), &renderTargetViewDesc, renderTargetViewArray[i].GetAddressOf());
		if (FAILED(hr))
		{
			MSG_BOX("Failed to create DeferredBuffer's render target view.");
			return E_FAIL;
		}
	}

	// 셰이더 리소스 뷰의 구조체를 설정합니다.
	D3D11_SHADER_RESOURCE_VIEW_DESC shaderResourceViewDesc;
	shaderResourceViewDesc.Format = textureDesc.Format;
	shaderResourceViewDesc.ViewDimension = D3D11_SRV_DIMENSION_TEXTURE2D;
	shaderResourceViewDesc.Texture2D.MostDetailedMip = 0;
	shaderResourceViewDesc.Texture2D.MipLevels = 1;

	// 셰이더 리소스 뷰를 만듭니다.
	for (int i = 0; i < BUFFER_END; i++)
	{
		hr = Graphic->GetDevice()->CreateShaderResourceView(renderTargetTextureArray[i].Get(), &shaderResourceViewDesc, shaderResourceViewArray[i].GetAddressOf());
		if (FAILED(hr))
		{
			MSG_BOX("Failed to create DeferredBuffer's shader resource view");
			return E_FAIL;
		}
	}

	// 깊이 버퍼의 구조체를 초기화합니다.
	D3D11_TEXTURE2D_DESC depthBufferDesc;
	ZeroMemory(&depthBufferDesc, sizeof(depthBufferDesc));

	// 깊이 버퍼의 구조체를 설정합니다.
	depthBufferDesc.Width = textureWidth;
	depthBufferDesc.Height = textureHeight;
	depthBufferDesc.MipLevels = 1;
	depthBufferDesc.ArraySize = 1;
	depthBufferDesc.Format = DXGI_FORMAT_D24_UNORM_S8_UINT;
	depthBufferDesc.SampleDesc.Count = 1;
	depthBufferDesc.SampleDesc.Quality = 0;
	depthBufferDesc.Usage = D3D11_USAGE_DEFAULT;
	depthBufferDesc.BindFlags = D3D11_BIND_DEPTH_STENCIL;
	depthBufferDesc.CPUAccessFlags = 0;
	depthBufferDesc.MiscFlags = 0;

	// 채워진 구조체를 사용하여 깊이 버퍼의 텍스처를 만듭니다.
	hr = Graphic->GetDevice()->CreateTexture2D(&depthBufferDesc, NULL, depthStencilBuffer.GetAddressOf());
	if (FAILED(hr))
	{
		MSG_BOX("Failed to create DeferredBuffer's depth buffer.");
		return E_FAIL;
	}

	// 깊이 스텐실 뷰 구조체를 초기화합니다.
	D3D11_DEPTH_STENCIL_VIEW_DESC depthStencilViewDesc;
	ZeroMemory(&depthStencilViewDesc, sizeof(depthStencilViewDesc));

	// 깊이 스텐실 뷰 구조체를 설정합니다.
	depthStencilViewDesc.Format = DXGI_FORMAT_D24_UNORM_S8_UINT;
	depthStencilViewDesc.ViewDimension = D3D11_DSV_DIMENSION_TEXTURE2D;
	depthStencilViewDesc.Texture2D.MipSlice = 0;

	// 깊이 스텐실 뷰를 만듭니다.
	hr = Graphic->GetDevice()->CreateDepthStencilView(depthStencilBuffer.Get(), &depthStencilViewDesc, depthStencilView.GetAddressOf());
	if (FAILED(hr))
	{
		MSG_BOX("Failed to create DeferredBuffer's depth stencil view.");
		return E_FAIL;
	}

	// 렌더링을 위해 뷰포트를 설정합니다.
	viewport.Width = (float)textureWidth;
	viewport.Height = (float)textureHeight;
	viewport.MinDepth = 0.0f;
	viewport.MaxDepth = 1.0f;
	viewport.TopLeftX = 0.0f;
	viewport.TopLeftY = 0.0f;

	return S_OK;
}

ID3D11ShaderResourceView * DeferredBuffer::GetShaderResourceView(int _view)
{
	return shaderResourceViewArray[_view].Get();
}

void DeferredBuffer::ClearRenderTargets(wstring MRTTag, float _red, float _green, float _blue, float _alpha)
{
	// 버퍼를 지울 색을 설정합니다.
	float color[4] = { _red, _green, _blue, _alpha };

	auto renderTargetVector = FindMultiRenderTarget(MRTTag);

	// 렌더 타겟 버퍼를 지웁니다.

	for (size_t i = 0; i< renderTargetVector->size(); ++i)
	{
		
		Graphic->GetDeviceContext()->ClearRenderTargetView(renderTargetVector->data()[i], color);
	}

	// 깊이 버퍼를 지웁니다.
	Graphic->GetDeviceContext()->ClearDepthStencilView(depthStencilView.Get(), D3D11_CLEAR_DEPTH, 1.0f, 0);
}

void DeferredBuffer::SetRenderTargets(wstring MRTTag)
{
	// 렌더링 대상 뷰 배열 및 깊이 스텐실 버퍼를 출력 렌더 파이프 라인에 바인딩 합니다.

	auto renderTargetVector = FindMultiRenderTarget(MRTTag);

	Graphic->GetDeviceContext()->OMSetRenderTargets(renderTargetVector->size(), renderTargetVector->data(), depthStencilView.Get());

	// 뷰포트를 설정합니다.
	Graphic->GetDeviceContext()->RSSetViewports(1, &viewport);
}

void DeferredBuffer::Render()
{
	for (int i = 0; i < BUFFER_END; ++i)
	{
		renderTargets[i]->Render(shaderResourceViewArray[i].Get());
	}

}

void DeferredBuffer::BeginMRT(wstring MRTTag)
{
	SetRenderTargets(MRTTag);
	
	ClearRenderTargets(MRTTag, 0.f, 0.f, 1.f, 1.f);
}

void DeferredBuffer::EndMRT()
{
	Graphic->SetBackBufferRenderTarget();

}

void DeferredBuffer::AddMultiRenderTarget(wstring MRTTag, BUFFER id)
{
	
	auto rtvVector = FindMultiRenderTarget(MRTTag);

	if (nullptr == rtvVector)
	{
		vector<ID3D11RenderTargetView*> rtvVector;

		rtvVector.push_back(renderTargetViewArray[id].Get());

		renderTargetMap.emplace(MRTTag, rtvVector);
	}
	else
		rtvVector->push_back(renderTargetViewArray[id].Get());

}

vector<ID3D11RenderTargetView*>* DeferredBuffer::FindMultiRenderTarget(wstring MRTTag)
{
	auto iter = renderTargetMap.find(MRTTag);


	if (iter == renderTargetMap.end())
		return nullptr;

	return &iter->second;
}

shared_ptr<DeferredBuffer> DeferredBuffer::Create(int _textureWidth, int _textureHeight, float _screenDepth, float _screenNear)
{
	shared_ptr<DeferredBuffer> Instance(new DeferredBuffer());
	if (FAILED(Instance->Initialize(_textureWidth, _textureHeight, _screenDepth, _screenNear)))
	{
		MSG_BOX("Failed to create DeferredBuffer.");
		return nullptr;
	}
	return Instance;
}
