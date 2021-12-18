#pragma once
#include "Component.h"

const int TEXTURE_REPEAT = 1;

class TerrainBufferTest : public Component
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

private:
	TerrainBufferTest();
	TerrainBufferTest(const TerrainBufferTest& Rhs);

public:
	~TerrainBufferTest();

public:
	virtual int Update(float _timeDelta) override;
	virtual void Render() override;
	virtual void RenderInspector() override;

public:
	int GetVertexCount();
	void CopyVertexArray(void* vertexList);

private:
	bool LoadHeightMap(const char* heightMapFilename);
	void NormalizeHeightMap();
	bool CalculateNormals();


	void CalculateTextureCoordinates();
	HRESULT InitializeBuffers();


private:
	HRESULT Initialize(const char* heightMapFilename);

public:
	static shared_ptr<TerrainBufferTest> Create(const char* heightMapFilename = nullptr);

private:
	int terrainWidth = 0;
	int terrainHeight = 0;
	HeightMapType* heightMap = nullptr;
	int vertexCount = 0;
	TerrainVertexType* vertices = nullptr;
};

