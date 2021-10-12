#include "pch.h"
#include "Mesh.h"
#include "Buffers.h"
#include "GraphicDevice.h"

Mesh::Mesh()
{
}

Mesh::Mesh(const Mesh & Rhs)
{
}

Mesh::~Mesh()
{
}

HRESULT Mesh::Initialize(wstring _name, int _boneIndex, wstring _materialName, shared_ptr<ModelVertex> _vertices, UINT _vertexCount, shared_ptr<UINT> _indices, UINT _indexCount)
{
	name = _name;
	boneIndex = _boneIndex;
	materialName = _materialName;
	vertices = _vertices;
	vertexCount = _vertexCount;
	indices = _indices;
	indexCount = _indexCount;

	vertexBuffer = VertexBuffer::Create(vertices.get(), vertexCount, sizeof(ModelVertex));
	indexBuffer = IndexBuffer::Create(indices.get(), indexCount);
	return S_OK;
}

void Mesh::Update()
{
	boneDesc.Index = boneIndex;
	
}

void Mesh::Render()
{
	vertexBuffer->Render();
	indexBuffer->Render();

	GraphicDevice::GetInstance()->GetDeviceContext()->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
}

shared_ptr<Mesh> Mesh::Create(wstring _name, int _boneIndex, wstring _materialName, shared_ptr<ModelVertex> _vertices, UINT _vertexCount, shared_ptr<UINT> _indices, UINT _indexCount)
{
	shared_ptr<Mesh> Instance(new Mesh());

	if (FAILED(Instance->Initialize(_name, _boneIndex, _materialName, _vertices, _vertexCount, _indices, _indexCount)))
	{
		MSG_BOX("Failed to create Mesh.");
		return nullptr;
	}
	return Instance;
}
