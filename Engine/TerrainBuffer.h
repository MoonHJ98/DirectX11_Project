#pragma once

#include "Includes.h"

class GraphicDevice;
class Management;

class TerrainBuffer
{
	struct VertexType
	{
		Vector3 position;
		Vector2 Uv;
		Vector3 normal;
	};

	struct BrushDesc
	{
		Color color = Color(0.f, 1.f, 0.f, 1.f);
		Vector3 position = Vector3(0.f, 0.f, 0.f);
		UINT range = 5;
	};

private:
	TerrainBuffer();
	TerrainBuffer(const TerrainBuffer& Rhs);

public:
	~TerrainBuffer();

private:
	HRESULT Initialize(HWND _hWnd, UINT _terrainWidth, UINT _terrainHeight);
	HRESULT InitializeBuffers();


public:
	void Render();
	VertexType* GetVertices() { return vertices; }
	Vector3 PickTerrain(Vector2 screenPos);


private:
	void RenderBuffers();

public:
	static shared_ptr<TerrainBuffer> Create(HWND _hWnd, UINT _terrainWidth, UINT _terrainHeight);

private:
	shared_ptr<GraphicDevice> Graphic = nullptr;
	ComPtr<ID3D11Buffer> vertexBuffer = nullptr;
	ComPtr<ID3D11Buffer> indexBuffer = nullptr;

	VertexType* vertices = nullptr;
	UINT* indices = nullptr;

	UINT terrainWidth = 0;
	UINT terrainHeight = 0;
	UINT screenWidth = 0;
	UINT screenHeignt = 0;

	UINT vertexCount = 0;
	UINT indexCount = 0;

	shared_ptr<Management> Manage = nullptr;
	HWND hWnd;

	BrushDesc brushDesc;
	shared_ptr<ConstantBuffer<BrushDesc>> brushBuffer = nullptr;

	//Vector2 screenPos = Vector2(0.f, 0.f);
};

