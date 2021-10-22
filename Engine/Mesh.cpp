#include "pch.h"
#include "Mesh.h"
#include "Material.h"
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

HRESULT Mesh::Initialize(wstring _name, int _boneIndex, wstring _materialName, vector<ModelVertex>& _vertices, vector<UINT>& _indices)
{
	Graphic = GraphicDevice::GetInstance();
	name = _name;
	boneIndex = _boneIndex;
	materialName = _materialName;
	//vertices = _vertices;
	vertexCount = _vertices.size();
	//indices = _indices;
	indexCount = _indices.size();

	CreateStaticBuffer(Graphic->GetDevice(), &_vertices[0], _vertices.size(), sizeof(ModelVertex), D3D11_BIND_VERTEX_BUFFER, vertexBuffer.GetAddressOf());
	CreateStaticBuffer(Graphic->GetDevice(), &_indices[0], _indices.size(), sizeof(UINT), D3D11_BIND_INDEX_BUFFER, indexBuffer.GetAddressOf());

	shared_ptr<ConstantBuffer<BoneDesc>> temp(new ConstantBuffer<BoneDesc>());
	BoneMatrixbuffer = temp;
	BoneMatrixbuffer->Create(Graphic->GetDevice());

	return S_OK;
}

void Mesh::Update()
{
	boneDesc.Index = boneIndex;
	
}

void Mesh::Render()
{
	UINT stride;
	UINT offset;

	BoneMatrixbuffer->SetData(Graphic->GetDeviceContext(), boneDesc);
	auto buffer = BoneMatrixbuffer->GetBuffer();
	Graphic->GetDeviceContext()->VSSetConstantBuffers(1, 1, &buffer);

	if (material)
		material->Render();
		
	if (transformsSRV)
		Graphic->GetDeviceContext()->VSSetShaderResources(0, 1, transformsSRV.GetAddressOf());


	stride = sizeof(ModelVertex);
	offset = 0;

	// Set the vertex buffer to active in the input assembler so it can be rendered.
	Graphic->GetDeviceContext()->IASetVertexBuffers(0, 1, vertexBuffer.GetAddressOf(), &stride, &offset);

	// Set the index buffer to active in the input assembler so it can be rendered.
	Graphic->GetDeviceContext()->IASetIndexBuffer(indexBuffer.Get(), DXGI_FORMAT_R32_UINT, 0);

	// Set the type of primitive that should be rendered from this vertex buffer, in this case triangles.
	Graphic->GetDeviceContext()->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);

	Graphic->GetDeviceContext()->DrawIndexed(indexCount, 0, 0);

}

void Mesh::SetTransforms(Matrix * transforms)
{
	memcpy(boneDesc.Transforms, transforms, sizeof(Matrix) * MAX_MODEL_TRANSFORMS);

}

shared_ptr<Mesh> Mesh::Create(wstring _name, int _boneIndex, wstring _materialName, vector<ModelVertex>& _vertices, vector<UINT>& _indices)
{
	shared_ptr<Mesh> Instance(new Mesh());

	if (FAILED(Instance->Initialize(_name, _boneIndex, _materialName, _vertices, _indices)))
	{
		MSG_BOX("Failed to create Mesh.");
		return nullptr;
	}
	return Instance;
}
