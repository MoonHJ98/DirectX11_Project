#pragma once

#include "Component.h"

class TerrainBuffer;
class TerrainBufferTest;
class Management;
class GraphicDevice;

const int MAX_TRIANGLES = 10000;

class Quadtree : public Component
{
private:
	struct NodeType
	{
		float positionX = 0.f;
		float positionZ = 0.f;
		float width = 0.f;
		int triangleCount = 0;
		ComPtr<ID3D11Buffer> vertexBuffer;
		ComPtr<ID3D11Buffer> indexBuffer;
		shared_ptr<NodeType> nodes[4] = { NULL };
	};

private:
	Quadtree();
	Quadtree(const Quadtree& Rhs);

public:
	~Quadtree();

public:
	// Base을(를) 통해 상속됨
	virtual int Update(float _timeDelta = 0) override;
	virtual void Render() override;
	virtual void RenderInspector() override;


private:
	HRESULT Initialize(shared_ptr<TerrainBufferTest> _terrainBuffer);
	void CalculateMeshDimensions(UINT _vertexCount, float& _centerX, float& _centerZ, float& _width);
	void CreateTreeNode(shared_ptr<NodeType>& _node, float _positionX, float _positionZ, float _width);
	int CountTriangles(float _positionX, float _positionZ, float _width);
	bool isTriangleContained(int _index, float _positionX, float _positionZ, float _width);
	void RenderNode(shared_ptr<NodeType> _node);

public:
	static shared_ptr<Quadtree> Create(shared_ptr<TerrainBufferTest> _terrainBuffer);

private:
	UINT triangleCount = 0;
	shared_ptr<TerrainVertexType> vertexList = nullptr;
	shared_ptr<NodeType> parentNode = nullptr;

	shared_ptr<Management> manage = nullptr;
	shared_ptr<GraphicDevice> graphic = nullptr;
};

