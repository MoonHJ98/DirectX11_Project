#include "pch.h"
#include "TextureBrush.h"
#include "GraphicDevice.h"
#include "Texture.h"
#include "TerrainBuffer.h"

TextureBrush::TextureBrush()
{
}

TextureBrush::TextureBrush(const TextureBrush & Rhs)
{
}

TextureBrush::~TextureBrush()
{
}

HRESULT TextureBrush::InitializeAlphaMap()
{
	D3D11_TEXTURE2D_DESC desc;
	ZeroMemory(&desc, sizeof(D3D11_TEXTURE2D_DESC));

	desc.Width = 256;
	desc.Height = 256;
	desc.MipLevels = 1;
	desc.ArraySize = 1;
	desc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
	desc.SampleDesc.Count = 1;
	desc.SampleDesc.Quality = 0;
	desc.Usage = D3D11_USAGE_DYNAMIC;
	desc.BindFlags = D3D11_BIND_SHADER_RESOURCE;
	desc.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
	desc.MiscFlags = 0;

	filterTextureHeight = desc.Height;
	filterTextureWidth = desc.Width;
	Graphic->GetDevice()->CreateTexture2D(&desc, nullptr, filterTexture.GetAddressOf());

	D3D11_MAPPED_SUBRESOURCE mappedResource;
	ZeroMemory(&mappedResource, sizeof(D3D11_MAPPED_SUBRESOURCE));

	if (SUCCEEDED(Graphic->GetDeviceContext()->Map(filterTexture.Get(), 0, D3D11_MAP_WRITE_DISCARD, 0, &mappedResource)))
	{
		LPBYTE pTexels = (LPBYTE)mappedResource.pData;

		for (UINT row = 0; row < desc.Height; ++row)
		{
			//int rowStart = row * mappedResource.RowPitch;
			LPDWORD pDWordDST = (LPDWORD)(pTexels + row * mappedResource.RowPitch);

			for (UINT col = 0; col < desc.Width; ++col)
			{
				*(pDWordDST + col) = 0x00000000;
			}
		}

		Graphic->GetDeviceContext()->Unmap(filterTexture.Get(), 0);

	}

	ScratchImage image;
	if (FAILED(CaptureTexture(Graphic->GetDevice(), Graphic->GetDeviceContext(), filterTexture.Get(), image)))
		return E_FAIL;

	filterTextureName = L"../Resources/TextureLayer.png";
	if (FAILED(SaveToWICFile(image.GetImages(), image.GetImageCount(), WIC_FLAGS_NONE, GetWICCodec(WIC_CODEC_PNG), filterTextureName.c_str(), nullptr)))
		return E_FAIL;

	return S_OK;
}

void TextureBrush::DrawAlphaMap(BrushDesc& _brushDesc, TerrainBuffer::TextureForPaintOption& _textureOption)
{
	// 알파맵의 한 픽셀이 월드 상의 한점의 크기를 구한다.
	float		PixSize = (float)_brushDesc.terrainWidth / (float)filterTextureWidth;

	//전역브러시 와 전역스무스는 현재 브러쉬할원의 지름을 넣기대문에 반지름을 사용해야한다.
	float 	    fHalfBrushSize = _brushDesc.range * 2 / 2.0f;
	float		fHalfSmoothSize = _brushDesc.smoothSize / 2.0f;

	// 에디터의 원의 최대 크기에 한점의 크기를 나누워서
	// 몇픽셀을 에디터 하는지 크기를 구한다.
	float		EditSize = fHalfBrushSize / PixSize;

	float	PosU = _brushDesc.position.x / (float)(_brushDesc.terrainWidth);
	float	PosV = 1 - _brushDesc.position.z / (float)(_brushDesc.terrainHeight);


	texPosX = int(PosU  * filterTextureWidth);
	texPosY = int(PosV  * filterTextureHeight);

	int StartPosX = int(((texPosX - EditSize) < 0) ? 0 : texPosX - EditSize);
	int StartPosY = int(((texPosY - EditSize) < 0) ? 0 : texPosY - EditSize);
	int EndPosX = int(((texPosX + EditSize) >= filterTextureWidth) ? filterTextureWidth - 1 : texPosX + EditSize);
	int EndPosY = int(((texPosY + EditSize) >= filterTextureHeight) ? filterTextureHeight - 1 : texPosY + EditSize);

	DWORD dwChangeColor = 0x00;
	float Smooth = 0.0f;
	DWORD dwA = 0x00;

	D3D11_MAPPED_SUBRESOURCE mappedResource;
	ZeroMemory(&mappedResource, sizeof(D3D11_MAPPED_SUBRESOURCE));

	if (SUCCEEDED(Graphic->GetDeviceContext()->Map(filterTexture.Get(), 0, D3D11_MAP_WRITE_DISCARD, 0, &mappedResource)))
	{

		//UCHAR* pDataDST = (UCHAR*)mappedResource.pData;
		UCHAR* pTexels = (UCHAR*)mappedResource.pData;

		LPBYTE pDataDST = (LPBYTE)mappedResource.pData;


		for (int j = StartPosY; j <= EndPosY; ++j)
		{
			//UINT rowStart = j * mappedResource.RowPitch;

			LPDWORD pDWordDST = (LPDWORD)(pDataDST + j * mappedResource.RowPitch);

			UINT rowStart = j * mappedResource.RowPitch;
			for (int i = StartPosX; i <= EndPosX; ++i)
			{
				UINT colStart = i * 4;
				Vector3 Pix = Vector3(i * PixSize, 0.0f, j * PixSize) - Vector3(texPosX * PixSize, 0.0f, texPosY * PixSize);
				float Len = Pix.Length();

				if (Len <= fHalfSmoothSize)
				{
					dwChangeColor = 0xFFFFFFFF;
				}
				else if (Len <= fHalfBrushSize)
				{
					// 최대사이즈에 포함될 우 최대사이즈까지의
					// 선영보간한 알파값을 구한다.
					Len -= fHalfSmoothSize;
					Smooth = fHalfBrushSize - fHalfSmoothSize;
					dwChangeColor = DWORD((Smooth - Len) / Smooth * 0xFF000000);
					dwA = (dwChangeColor & 0xFF000000) >> 24;
					dwChangeColor = (dwA << 24) | (dwA << 16) | (dwA << 8) | dwA;
				}
				else
				{
					continue;
				}

				//*(pDWordDST + i) = (*(pDWordDST + i) < dwChangeColor) ? dwChangeColor : *(pDWordDST + i);

				switch (_textureOption)
				{
				case TerrainBuffer::TextureForPaintOption::Texture1:
					pTexels[rowStart + colStart + 0] = (*(pDWordDST + i) < dwChangeColor) ? (UCHAR)dwChangeColor : *(UCHAR*)(pDWordDST + i);
					pTexels[rowStart + colStart + 1] = pTexels[rowStart + colStart + 1];
					pTexels[rowStart + colStart + 2] = pTexels[rowStart + colStart + 2];
					pTexels[rowStart + colStart + 3] = (*(pDWordDST + i) < dwChangeColor) ? (UCHAR)dwChangeColor : *(UCHAR*)(pDWordDST + i);
					break;
				case TerrainBuffer::TextureForPaintOption::Texture2:
					pTexels[rowStart + colStart + 0] = (*(pDWordDST + i) < dwChangeColor) ? (UCHAR)dwChangeColor : *(UCHAR*)(pDWordDST + i);
					pTexels[rowStart + colStart + 1] = (*(pDWordDST + i) < dwChangeColor) ? (UCHAR)dwChangeColor : *(UCHAR*)(pDWordDST + i);
					pTexels[rowStart + colStart + 2] = pTexels[rowStart + colStart + 2];
					pTexels[rowStart + colStart + 3] = (*(pDWordDST + i) < dwChangeColor) ? (UCHAR)dwChangeColor : *(UCHAR*)(pDWordDST + i);
					break;
				case TerrainBuffer::TextureForPaintOption::Texture3:
					pTexels[rowStart + colStart + 0] = (*(pDWordDST + i) < dwChangeColor) ? (UCHAR)dwChangeColor : *(UCHAR*)(pDWordDST + i);
					pTexels[rowStart + colStart + 1] = pTexels[rowStart + colStart + 1];
					pTexels[rowStart + colStart + 2] = (*(pDWordDST + i) < dwChangeColor) ? (UCHAR)dwChangeColor : *(UCHAR*)(pDWordDST + i);
					pTexels[rowStart + colStart + 3] = (*(pDWordDST + i) < dwChangeColor) ? (UCHAR)dwChangeColor : *(UCHAR*)(pDWordDST + i);
					break;
				default:
					break;
				}

			}
		}

		Graphic->GetDeviceContext()->Unmap(filterTexture.Get(), 0);
	}

	ScratchImage image;
	CaptureTexture(Graphic->GetDevice(), Graphic->GetDeviceContext(), filterTexture.Get(), image);

	SaveToWICFile(image.GetImages(), image.GetImageCount(), WIC_FLAGS_NONE, GetWICCodec(WIC_CODEC_PNG), filterTextureName.c_str(), nullptr);



}

void TextureBrush::Render()
{
	Graphic->GetDeviceContext()->PSSetShaderResources(1, 1, filterTextureView.GetAddressOf());
}


HRESULT TextureBrush::Initialize()
{
	Graphic = GraphicDevice::GetInstance();



	if (FAILED(InitializeAlphaMap()))
		return E_FAIL;

	if (filterTextureView == nullptr)
	{
		D3D11_SHADER_RESOURCE_VIEW_DESC SRVDesc;
		memset(&SRVDesc, 0, sizeof(SRVDesc));
		SRVDesc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
		SRVDesc.ViewDimension = D3D11_SRV_DIMENSION_TEXTURE2D;
		SRVDesc.Texture2D.MipLevels = 1;

		Graphic->GetDevice()->CreateShaderResourceView(filterTexture.Get(), &SRVDesc, filterTextureView.GetAddressOf());
	}

	return S_OK;
}

shared_ptr<TextureBrush> TextureBrush::Create()
{
	shared_ptr<TextureBrush> Instance(new TextureBrush());
	if (FAILED(Instance->Initialize()))
	{
		MSG_BOX("Failed to create TextureBrush");
		return nullptr;
	}
	return Instance;
}
