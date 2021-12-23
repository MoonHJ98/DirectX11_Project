#pragma once

#include "Includes.h"

class DebugTreeRenderer;
class Management;

class DebugTree
{
private:
	DebugTree();
	DebugTree(const DebugTree& Rhs);

public:
	~DebugTree();

private:
	HRESULT Initialize(shared_ptr<NodeType> _rootNode, shared_ptr<TerrainVertexType> _vertices);
	HRESULT CreateBoundingBox(shared_ptr<NodeType> _node);
	HRESULT CreateDebugTreeRenderer(BoundingBox& _boundingBox);

public:
	void Render();
	Vector3 PickTerrain(Vector2 _screenCursorPos, Vector2 _screenSize);

public:
	static shared_ptr<DebugTree> Create(shared_ptr<NodeType> _rootNode, shared_ptr<TerrainVertexType> _vertices);

private:
	vector<BoundingBox> boundingBox;
	vector<shared_ptr<DebugTreeRenderer>> debugTreeRenderers;
	shared_ptr<Management> Manage = nullptr;
	weak_ptr<TerrainVertexType> vertices;
	
	   
};

