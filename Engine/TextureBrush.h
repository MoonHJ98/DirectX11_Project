#pragma once

#include "Includes.h"
#include "TerrainBuffer.h"

class GraphicDevice;
class Texture;


class TextureBrush
{
private:
	TextureBrush();
	TextureBrush(const TextureBrush& Rhs);

public:
	~TextureBrush();

private:
	HRESULT InitializeAlphaMap();

public:
	void DrawAlphaMap(BrushDesc& _brushDesc, TerrainBuffer::TextureForPaintOption& _textureOption);
	void Render();

private:
	HRESULT Initialize();

public:
	static shared_ptr<TextureBrush> Create();

private:
	shared_ptr<GraphicDevice> Graphic = nullptr;
	
	ComPtr<ID3D11Texture2D> filterTexture = nullptr;

	wstring filterTextureName;
	ComPtr<ID3D11ShaderResourceView> filterTextureView = nullptr;

	int filterTextureHeight = 0;
	int filterTextureWidth = 0;
	int texPosX = 0;
	int texPosY = 0;
};

