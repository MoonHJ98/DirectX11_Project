#include "pch.h"
#include "DefferedBuffer.h"
#include "GraphicDevice.h"


DefferedBuffer::DefferedBuffer()
{
}

DefferedBuffer::DefferedBuffer(const DefferedBuffer & Rhs)
{
}

DefferedBuffer::~DefferedBuffer()
{
}

HRESULT DefferedBuffer::Initialize(int _textureWidth, int _textureHeight, float _screenDepth, float _screenNear)
{
	HRESULT hr;
	textureWidth = _textureWidth;
	textureHeight = _textureHeight;

	// ���� Ÿ�� �ؽ�ó ����ü�� �ʱ�ȭ�մϴ�.
	D3D11_TEXTURE2D_DESC textureDesc;
	ZeroMemory(&textureDesc, sizeof(textureDesc));

	// ���� Ÿ�� �ؽ�ó ����ü�� �����մϴ�.
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


	// ���� Ÿ�� �ؽ�ó�� ����ϴ�.
	for (int i = 0; i < BUFFER_COUNT; i++)
	{
		hr = Graphic->GetDevice()->CreateTexture2D(&textureDesc, NULL, renderTargetTextureArray[i].GetAddressOf());
		if (FAILED(hr))
		{
			MSG_BOX("Failed to craete DefferedBuffer's render target texture.");
			return E_FAIL;
		}
	}

	// ���� Ÿ�� ���� ����ü�� �����մϴ�.
	D3D11_RENDER_TARGET_VIEW_DESC renderTargetViewDesc;
	renderTargetViewDesc.Format = textureDesc.Format;
	renderTargetViewDesc.ViewDimension = D3D11_RTV_DIMENSION_TEXTURE2D;
	renderTargetViewDesc.Texture2D.MipSlice = 0;

	// ���� Ÿ�� �並 �����մϴ�.
	for (int i = 0; i < BUFFER_COUNT; i++)
	{
		hr = Graphic->GetDevice()->CreateRenderTargetView(renderTargetTextureArray[i].Get(), &renderTargetViewDesc, renderTargetViewArray[i].GetAddressOf());
		if (FAILED(hr))
		{
			MSG_BOX("Failed to create DefferedBuffer's render target view.");
			return E_FAIL;
		}
	}

	// ���̴� ���ҽ� ���� ����ü�� �����մϴ�.
	D3D11_SHADER_RESOURCE_VIEW_DESC shaderResourceViewDesc;
	shaderResourceViewDesc.Format = textureDesc.Format;
	shaderResourceViewDesc.ViewDimension = D3D11_SRV_DIMENSION_TEXTURE2D;
	shaderResourceViewDesc.Texture2D.MostDetailedMip = 0;
	shaderResourceViewDesc.Texture2D.MipLevels = 1;

	// ���̴� ���ҽ� �並 ����ϴ�.
	for (int i = 0; i < BUFFER_COUNT; i++)
	{
		hr = Graphic->GetDevice()->CreateShaderResourceView(renderTargetTextureArray[i].Get(), &shaderResourceViewDesc, shaderResourceViewArray[i].GetAddressOf());
		if (FAILED(hr))
		{
			MSG_BOX("Failed to create DefferedBuffer's shader resource view");
			return E_FAIL;
		}
	}

	// ���� ������ ����ü�� �ʱ�ȭ�մϴ�.
	D3D11_TEXTURE2D_DESC depthBufferDesc;
	ZeroMemory(&depthBufferDesc, sizeof(depthBufferDesc));

	// ���� ������ ����ü�� �����մϴ�.
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

	// ä���� ����ü�� ����Ͽ� ���� ������ �ؽ�ó�� ����ϴ�.
	hr = Graphic->GetDevice()->CreateTexture2D(&depthBufferDesc, NULL, depthStencilBuffer.GetAddressOf());
	if (FAILED(hr))
	{
		MSG_BOX("Failed to create DefferedBuffer's depth buffer.");
		return E_FAIL;
	}

	// ���� ���ٽ� �� ����ü�� �ʱ�ȭ�մϴ�.
	D3D11_DEPTH_STENCIL_VIEW_DESC depthStencilViewDesc;
	ZeroMemory(&depthStencilViewDesc, sizeof(depthStencilViewDesc));

	// ���� ���ٽ� �� ����ü�� �����մϴ�.
	depthStencilViewDesc.Format = DXGI_FORMAT_D24_UNORM_S8_UINT;
	depthStencilViewDesc.ViewDimension = D3D11_DSV_DIMENSION_TEXTURE2D;
	depthStencilViewDesc.Texture2D.MipSlice = 0;

	// ���� ���ٽ� �並 ����ϴ�.
	hr = Graphic->GetDevice()->CreateDepthStencilView(depthStencilBuffer.Get(), &depthStencilViewDesc, depthStencilView.GetAddressOf());
	if (FAILED(hr))
	{
		MSG_BOX("Failed to create DefferedBuffer's depth stencil view.");
		return E_FAIL;
	}

	// �������� ���� ����Ʈ�� �����մϴ�.
	viewport.Width = (float)textureWidth;
	viewport.Height = (float)textureHeight;
	viewport.MinDepth = 0.0f;
	viewport.MaxDepth = 1.0f;
	viewport.TopLeftX = 0.0f;
	viewport.TopLeftY = 0.0f;

	return S_OK;
}

ID3D11ShaderResourceView * DefferedBuffer::GetShaderResourceView(int _view)
{
	return shaderResourceViewArray[_view].Get();
}

void DefferedBuffer::ClearRenderTargets(float _red, float _green, float _blue, float _alpha)
{
}

void DefferedBuffer::SetRenderTargets()
{
	// ������ ��� �� �迭 �� ���� ���ٽ� ���۸� ��� ���� ������ ���ο� ���ε� �մϴ�.
	ID3D11RenderTargetView* rtv[2] =
	{ 
		renderTargetViewArray[0].Get(),
	    renderTargetViewArray[1].Get()
	};
	Graphic->GetDeviceContext()->OMSetRenderTargets(BUFFER_COUNT, rtv, depthStencilView.Get());

	// ����Ʈ�� �����մϴ�.
	Graphic->GetDeviceContext()->RSSetViewports(1, &viewport);
}

shared_ptr<DefferedBuffer> DefferedBuffer::Create(int _textureWidth, int _textureHeight, float _screenDepth, float _screenNear)
{
	shared_ptr<DefferedBuffer> Instance(new DefferedBuffer());
	if (FAILED(Instance->Initialize(_textureWidth, _textureHeight, _screenDepth, _screenNear)))
	{
		MSG_BOX("Failed to create DefferedBuffer.");
		return nullptr;
	}
	return Instance;
}
