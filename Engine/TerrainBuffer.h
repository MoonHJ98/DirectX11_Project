#pragma once
#include "Component.h"

const int TEXTURE_REPEAT = 1;

class Management;
class GraphicDevice;

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

public:
	virtual int Update(float _timeDelta) override;
	virtual void Render() override;
	virtual void RenderInspector() override;

public:
	int GetVertexCount();
	void CopyVertexArray(void* vertexList);
	Vector3 PickTerrain(Vector2 screenPos);
	void SetScreenSize(Vector2 _screenSize) { screenWidth = (UINT)_screenSize.x; screenHeignt = (UINT)_screenSize.y; }

private:
	bool LoadHeightMap(const char* heightMapFilename);
	void NormalizeHeightMap();
	bool CalculateNormals();


	void CalculateTextureCoordinates();
	HRESULT InitializeBuffers();


private:
	HRESULT Initialize(const char* heightMapFilename);

public:
	static shared_ptr<TerrainBuffer> Create(const char* heightMapFilename = nullptr);

private:
	shared_ptr<Management> Manage = nullptr;
	shared_ptr<GraphicDevice> Graphic = nullptr;

	int terrainWidth = 0;
	int terrainHeight = 0;
	HeightMapType* heightMap = nullptr;
	int vertexCount = 0;
	TerrainVertexType* vertices = nullptr;

	UINT screenWidth = 0;
	UINT screenHeignt = 0;

	BrushDesc brushDesc;
	shared_ptr<ConstantBuffer<BrushDesc>> brushBuffer = nullptr;
};

