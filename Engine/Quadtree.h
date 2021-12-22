#pragma once

#include "Component.h"

class TerrainBuffer;
class Management;
class GraphicDevice;
class DebugTree;

const int MAX_TRIANGLES = 10000;

class Quadtree : public Component
{
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
	HRESULT Initialize(shared_ptr<TerrainBuffer> _terrainBuffer);
	void CalculateMeshDimensions(UINT _vertexCount, float& _centerX, float& _centerZ, float& _width);
	void CreateTreeNode(shared_ptr<NodeType>& _node, float _positionX, float _positionZ, float _width);
	int CountTriangles(float _positionX, float _positionZ, float _width);
	bool isTriangleContained(int _index, float _positionX, float _positionZ, float _width);
	void RenderNode(shared_ptr<NodeType> _node);

public:
	static shared_ptr<Quadtree> Create(shared_ptr<TerrainBuffer> _terrainBuffer);

private:
	UINT triangleCount = 0;
	shared_ptr<TerrainVertexType> vertexList = nullptr;
	shared_ptr<NodeType> parentNode = nullptr;

	shared_ptr<Management> manage = nullptr;
	shared_ptr<GraphicDevice> graphic = nullptr;

	shared_ptr<DebugTree> debugTree = nullptr;
};

