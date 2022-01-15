#pragma once

#include "Component.h"

#define TEXTURE_REPEAT 1

class GraphicDevice;
class Management;
class HeightBrush;
class Texture;
class TextureBrush;

class TerrainBuffer : public Component, public enable_shared_from_this<TerrainBuffer>
{
private:
	struct TerrainToolDesc
	{
		BOOL _terrainTool = FALSE;
		BOOL padding[3] = {};
	};

	struct TextureInfoBufferDesc
	{
		BOOL useAlpha = FALSE;
		BOOL padding[3] = {};
	};

public:
	enum TerrainToolStyle { RaiseOrLowerTerrain, PaintTexture, TerrainToolStyleEnd };
	enum HeightMapOption { Circle, Mountain, DesertMountain, HeigitMapOptionEnd };
	enum TextureForPaintOption { Texture1, Texture2, Texture3, TextureForPaintOptionEnd };

private:
	TerrainBuffer();
	TerrainBuffer(const TerrainBuffer& Rhs);

public:
	~TerrainBuffer();

public:
	virtual int Update(float _timeDelta) override;
	virtual void Render() override;
	virtual void RenderInspector() override;

private:
	HRESULT Initialize(UINT _terrainWidth, UINT _terrainHeight, HWND _hWnd, const char* heightMapFilename);
	HRESULT InitializeBuffers();
	void RenderBuffers();
	void CreateNormalData();
	void HeightMapSelect();
	void TextureForPaintSelect();

public:
	TerrainVertexType* GetVertices() { return vertices; }
	Vector3 PickTerrain(Vector2 screenPos);
	int GetVertexCount();
	int GetIndexCount();
	void CopyVertexArray(void* vertexList);
	void CopyIndexArray(void* indexList);
	void RaiseHeight();
	void SetScreenSize(Vector2 _sceneSize);

	BOOL GetTerrainTool() { return terrainTool; }
	void SetTerrainTool(BOOL _terrainTool) { terrainTool = _terrainTool; }

	void SetChangeTerrainToolRender(bool _bCheck) { changeTerrainToolRender = _bCheck; }

	int GetTerrainWidth() { return terrainWidth; }
	int GetTerrainHeight() { return terrainHeight; }
	void DrawAlphaMap();
	int GetTerrainToolStyle() { return terrainToolStyle; }


public:
	static shared_ptr<TerrainBuffer> Create(UINT _terrainWidth, UINT _terrainHeight, HWND _hWnd, const char* heightMapFilename = nullptr);

private:
	shared_ptr<Management> Manage = nullptr;
	shared_ptr<GraphicDevice> Graphic = nullptr;
	ComPtr<ID3D11Buffer> vertexBuffer = nullptr;
	ComPtr<ID3D11Buffer> indexBuffer = nullptr;

	TerrainVertexType* vertices = nullptr;
	vector<UINT> indices;

	int terrainWidth = 0;
	int terrainHeight = 0;
	UINT screenWidth = 0;
	UINT screenHeignt = 0;

	UINT vertexCount = 0;
	UINT indexCount = 0;

	float timeDelta = 0;


	BrushDesc brushDesc;
	shared_ptr<ConstantBuffer<BrushDesc>> brushBuffer = nullptr;

	TerrainToolDesc toolDesc;
	shared_ptr<ConstantBuffer<TerrainToolDesc>> toolBuffer = nullptr;

	TextureInfoBufferDesc textureInfoBufferDesc;
	shared_ptr<ConstantBuffer<TextureInfoBufferDesc>> textureInfoBuffer = nullptr;

private:
	// For Raise Height
	shared_ptr<HeightBrush> mountain = nullptr;
	shared_ptr<HeightBrush> desertMountain = nullptr;
	shared_ptr<Texture> mountainTexture = nullptr;
	shared_ptr<Texture> desertMountainTexture = nullptr;
	shared_ptr<Texture> circleTexture = nullptr;
	int heightMapTextureCount = 0;
	ComPtr<ID3D11ShaderResourceView> heightMapTexture = nullptr;
	HeightMapOption heightMapOption = Circle;

	BOOL terrainTool = FALSE;
	bool changeTerrainToolRender = true;

	int terrainToolStyle = TerrainToolStyleEnd;

private:
	// For Paint Texture
	vector<shared_ptr<Texture>> textures;
	shared_ptr<TextureBrush> textureBrush;

	ComPtr<ID3D11ShaderResourceView> selectedTextureForPaint = nullptr;

	int textureOption = Texture1;

	int filterTextureHeight = 0;
	int filterTextureWidth = 0;
	float brushSize = 10.f;
	float smoothSize = 5.f;
	int texPosX = 0;
	int texPosY = 0;

	HWND hWnd;
};

