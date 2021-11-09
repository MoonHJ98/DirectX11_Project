#pragma once

#include "Includes.h"

class GraphicDevice;

class HeightTerrainBuffer
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
	HeightTerrainBuffer();
	HeightTerrainBuffer(const HeightTerrainBuffer& Rhs);

public:
	~HeightTerrainBuffer();

private:
	HRESULT Initialize(int _terrainWidth, int _terrainHeight, wstring _heightMapPath);
	HRESULT InitializeBuffer();
	HRESULT LoadHeightMap(wstring _heightMapPath);
	void SetTerrainCoordinates();
	HRESULT BuildTerrainModel();
	HRESULT CalculateNormal();


	void RenderBuffers();

public:
	void Render();

public:
	static shared_ptr<HeightTerrainBuffer> Create(int _terrainWidth, int _terrainHeight, wstring _heightMapPath);

private:
	shared_ptr<GraphicDevice> Graphic = nullptr;

	ComPtr<ID3D11Buffer> vertexBuffer = nullptr;
	ComPtr<ID3D11Buffer> indexBuffer = nullptr;

	HeightMapType* heightMap = nullptr;

	int vertexCount = 0;
	int indexCount = 0;
	int terrainWidth = 257;
	int terrainHeight = 257;
	float heightScale = 5.f;
	ModelType* terrainModel = nullptr;
};

