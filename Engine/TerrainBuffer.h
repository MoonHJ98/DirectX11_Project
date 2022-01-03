#pragma once

#include "Component.h"

#define TEXTURE_REPEAT 1

class GraphicDevice;
class Management;
class HeightBrush;
class Texture;

class TerrainBuffer : public Component
{
private:
	struct HeightMapType
	{
		float x, y, z;
		float tu, tv;
		float nx, ny, nz;
	};
	struct VectorType
	{
		float x, y, z;
	};

	struct TerrainToolDesc
	{
		BOOL _terrainTool = FALSE;
		BOOL padding[3] = {};
	};

	enum HeightMapOption { Circle, Mountain, DesertMountain, HeigitMapOptionEnd };

	enum TerrainToolStyle { RaiseOrLowerTerrain, PaintTexture, TerrainToolStyleEnd };

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
	HRESULT Initialize(UINT _terrainWidth, UINT _terrainHeight, const char* heightMapFilename);
	HRESULT InitializeBuffers();
	void RenderBuffers();
	bool LoadHeightMap(const char* heightMapFilename);
	void NormalizeHeightMap();
	bool CalculateNormals();
	void CalculateTextureCoordinates();
	void CreateNormalData();

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
public:
	static shared_ptr<TerrainBuffer> Create(UINT _terrainWidth, UINT _terrainHeight, const char* heightMapFilename = nullptr);

private:
	shared_ptr<Management> Manage = nullptr;
	shared_ptr<GraphicDevice> Graphic = nullptr;
	ComPtr<ID3D11Buffer> vertexBuffer = nullptr;
	ComPtr<ID3D11Buffer> indexBuffer = nullptr;

	TerrainVertexType* vertices = nullptr;
	UINT* indices = nullptr;

	HeightMapType* heightMap = nullptr;

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
};

