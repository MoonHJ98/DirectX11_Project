#pragma once

#include "Component.h"

class TerrainBuffer;
class Management;
class GraphicDevice;
class DebugTree;

const int MAX_TRIANGLES = 1000;

class Quadtree : public Component
{
private:
	Quadtree();
	Quadtree(const Quadtree& Rhs);

public:
	~Quadtree();

public:
	// Base��(��) ���� ��ӵ�
	virtual int Update(float _timeDelta = 0) override;
	virtual void Render() override;
	virtual void RenderInspector() override;

public:
	Vector3 PickTerrain(Vector2 _screenCursorPos, Vector2 _screenSize);


private:
	HRESULT Initialize(shared_ptr<TerrainBuffer> _terrainBuffer);
	void CalculateMeshDimensions(UINT _vertexCount, float& _centerX, float& _centerZ, float& _width);
	void CreateTreeNode(shared_ptr<NodeType>& _node, float _positionX, float _positionZ, float _width);
	int CountTriangles(float _positionX, float _positionZ, float _width);
	bool isTriangleContained(int _index, float _positionX, float _positionZ, float _width);
	void RenderNode(shared_ptr<NodeType> _node);
	void isTriangleContained(float _positionX, float _positionZ, float _width, vector<TerrainVertexType>& vertices, vector<UINT>& indices);

public:
	static shared_ptr<Quadtree> Create(shared_ptr<TerrainBuffer> _terrainBuffer);

private:
	UINT triangleCount = 0;
	shared_ptr<TerrainVertexType> vertexList = nullptr;
	shared_ptr<UINT> indexList = nullptr;

	shared_ptr<NodeType> parentNode = nullptr;

	shared_ptr<Management> manage = nullptr;
	shared_ptr<GraphicDevice> graphic = nullptr;

	shared_ptr<DebugTree> debugTree = nullptr;

	int vertexCount = 0;
	int indexCount = 0;
};

