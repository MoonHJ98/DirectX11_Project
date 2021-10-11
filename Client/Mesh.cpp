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

HRESULT Mesh::Initialize()
{
	vertexBuffer = VertexBuffer::Create(vertices, vertexCount, sizeof(ModelVertex));
	indexBuffer = IndexBuffer::Create(indices, indexCount);
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

void Mesh::Transforms(Matrix * transforms)
{
}

shared_ptr<Mesh> Mesh::Create()
{
	shared_ptr<Mesh> Instance(new Mesh());

	if (FAILED(Instance->Initialize()))
	{
		MSG_BOX("Failed to create Mesh.");
		return nullptr;
	}
	return Instance;
}
