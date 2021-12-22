#pragma once

#include "Includes.h"

class DebugTreeRenderer;

class DebugTree
{
private:
	DebugTree();
	DebugTree(const DebugTree& Rhs);

public:
	~DebugTree();

private:
	HRESULT Initialize(shared_ptr<NodeType> _rootNode);
	HRESULT CreateBoundingBox(shared_ptr<NodeType> _node);
	HRESULT CreateDebugTreeRenderer(BoundingBox& _boundingBox);

public:
	void Render();

public:
	static shared_ptr<DebugTree> Create(shared_ptr<NodeType> _rootNode);

private:
	vector<BoundingBox> boundingBox;
	vector<shared_ptr<DebugTreeRenderer>> debugTreeRenderers;



};

