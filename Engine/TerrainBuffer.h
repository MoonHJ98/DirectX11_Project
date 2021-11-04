#pragma once

#include "Includes.h"

class GraphicDevice;

class TerrainBuffer
{
	typedef struct VertexType
	{
		Vector3	position;
		Vector2 uv;
		Vector3 normal;

	}VERTEXTYPE;

	struct HeightMapType
	{
		float x, y, z;
		float nx, ny, nz;
	};
	struct ModelType
	{
		float x, y, z;
		float u, v;
		float nx, ny, nz;
	};
	struct VectorType
	{
		float x, y, z;
	};

private:
	TerrainBuffer();
	TerrainBuffer(const TerrainBuffer& Rhs);

public:
	~TerrainBuffer();

private:
	HRESULT Initialize(UINT _terrainWidth, UINT _terrainHeight, wstring _heightMapPath);
	HRESULT InitializeBuffer();
	HRESULT LoadHeightMap(wstring _heightMapPath);
	void SetTerrainCoordinates();
	HRESULT BuildTerrainModel();
	HRESULT CalculateNormal();


	void RenderBuffers();

public:
	void Render();

public:
	static shared_ptr<TerrainBuffer> Create(UINT _terrainWidth, UINT _terrainHeight, wstring _heightMapPath);

private:
	shared_ptr<GraphicDevice> Graphic = nullptr;

	ComPtr<ID3D11Buffer> vertexBuffer = nullptr;
	ComPtr<ID3D11Buffer> indexBuffer = nullptr;

	HeightMapType* heightMap = nullptr;

	UINT vertexCount = 0;
	UINT indexCount = 0;
	UINT terrainWidth = 257;
	UINT terrainHeight = 257;
	float heightScale = 12.f;
	ModelType* terrainModel = nullptr;
};

