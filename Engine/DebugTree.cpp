#include "pch.h"
#include "DebugTree.h"
#include "GraphicDevice.h"
#include "Shader.h"
#include "Transform.h"
#include "DebugTreeRenderer.h"

DebugTree::DebugTree()
{
}

DebugTree::DebugTree(const DebugTree & Rhs)
{
}

DebugTree::~DebugTree()
{
}

HRESULT DebugTree::Initialize(shared_ptr<NodeType> _rootNode)
{
	if (FAILED(CreateBoundingBox(_rootNode)))
	{
		MSG_BOX("Failed to create BoundingBox for DebugTree.");
		return E_FAIL;
	}

	for (size_t i = 0; i < boundingBox.size(); ++i)
	{

		if (FAILED(CreateDebugTreeRenderer(boundingBox[i])))
		{
			MSG_BOX("Failed to create DebugTreeRenderer for DebugTree.");
			return E_FAIL;
		}
	}
	

	return S_OK;
}

HRESULT DebugTree::CreateBoundingBox(shared_ptr<NodeType> _node)
{
	if (_node == nullptr)
		return S_OK;

	boundingBox.push_back(BoundingBox(Vector3(_node->positionX, 0.f, _node->positionZ), Vector3(_node->width / 2.f, 20.f, _node->width / 2.f)));

	

	for (int i = 0; i < 4; ++i)
		CreateBoundingBox(_node->nodes[i]);

	return S_OK;
}

HRESULT DebugTree::CreateDebugTreeRenderer(BoundingBox & _boundingBox)
{
	auto debugTreeRenderer = DebugTreeRenderer::Create(_boundingBox);
	
	debugTreeRenderers.push_back(debugTreeRenderer);

	return S_OK;
}


void DebugTree::Render()
{
	for (UINT i = 0; i < debugTreeRenderers.size(); ++i)
	{
		debugTreeRenderers[i]->Render();
	}
}

shared_ptr<DebugTree> DebugTree::Create(shared_ptr<NodeType> _rootNode)
{
	shared_ptr<DebugTree> Instance(new DebugTree());
	if (FAILED(Instance->Initialize(_rootNode)))
	{
		MSG_BOX("Failed to create DebugTree.");
		return nullptr;
	}
	return Instance;
}
