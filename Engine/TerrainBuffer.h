#pragma once

#include "Includes.h"

class GraphicDevice;

class TerrainBuffer
{
	struct VertexType
	{
		Vector3 position;
		Vector2 Uv;
		Vector3 normal;
	};

private:
	TerrainBuffer();
	TerrainBuffer(const TerrainBuffer& Rhs);

public:
	~TerrainBuffer();

private:
	HRESULT Initialize(UINT _terrainWidth, UINT _terrainHeight);
	HRESULT InitializeBuffers();

public:
	void Render();
	VertexType* GetVertices() { return vertices; }

private:
	void RenderBuffers();

public:
	static shared_ptr<TerrainBuffer> Create(UINT _terrainWidth, UINT _terrainHeight);

private:
	shared_ptr<GraphicDevice> Graphic = nullptr;
	ComPtr<ID3D11Buffer> vertexBuffer = nullptr;
	ComPtr<ID3D11Buffer> indexBuffer = nullptr;

	VertexType* vertices = nullptr;
	UINT* indices = nullptr;


	Vector3* Position = nullptr;

	UINT terrainWidth = 0;
	UINT terrainHeight = 0;

	UINT vertexCount = 0;
	UINT indexCount = 0;




};

