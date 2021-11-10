#include "pch.h"
#include "TerrainBuffer.h"
#include "GraphicDevice.h"

TerrainBuffer::TerrainBuffer()
{
}

TerrainBuffer::TerrainBuffer(const TerrainBuffer & Rhs)
{
}

TerrainBuffer::~TerrainBuffer()
{
}

HRESULT TerrainBuffer::Initialize(UINT _terrainWidth, UINT _terrainHeight)
{
	Graphic = GraphicDevice::GetInstance();
	terrainWidth = _terrainWidth;
	terrainHeight = _terrainHeight;

	
	return InitializeBuffers();
}

HRESULT TerrainBuffer::InitializeBuffers()
{
	vector<VertexType> v;


	vertexCount = terrainWidth * terrainHeight;
	Position = new Vector3[vertexCount];
	vertices = new VertexType[vertexCount];


	for (UINT z = 0; z < terrainHeight; ++z)
	{
		for (UINT x = 0; x < terrainWidth; ++x)
		{

			UINT index = terrainWidth * z + x;
			vertices[index].position = Vector3((float)x, 0.f, (float)z);
			vertices[index].normal = Vector3(0.f, 1.f, 0.f);
			vertices[index].Uv = Vector2((float)x / (float)terrainWidth, (float)(terrainHeight - 1 - z) / (float)terrainHeight);

		}
	}
	
	vector<UINT> indices;

	for (UINT z = 0; z < terrainHeight - 1; ++z)
	{
		for (UINT x = 0; x < terrainWidth - 1; ++x)
		{
			indices.push_back(terrainWidth * z + x);
			indices.push_back(terrainWidth * (z + 1) + x);
			indices.push_back(terrainWidth * z + x + 1);

			indices.push_back(terrainWidth * z + x + 1);
			indices.push_back(terrainWidth * (z + 1) + x);
			indices.push_back(terrainWidth * (z + 1) + x + 1);
		}
	}

	indexCount = indices.size();
	this->indices = new UINT[indexCount];

	copy(indices.begin(), indices.end(), this->indices);



	CreateStaticBuffer(Graphic->GetDevice(), vertices, vertexCount, sizeof(VertexType), D3D11_BIND_VERTEX_BUFFER, vertexBuffer.GetAddressOf());
	CreateStaticBuffer(Graphic->GetDevice(), this->indices, indexCount, sizeof(UINT), D3D11_BIND_INDEX_BUFFER, indexBuffer.GetAddressOf());

	return S_OK;
}

void TerrainBuffer::Render()
{


	RenderBuffers();
}

void TerrainBuffer::RenderBuffers()
{
	// 정점 버퍼 보폭 및 오프셋을 설정합니다.
	unsigned int stride = sizeof(VertexType);
	unsigned int offset = 0;

	// 렌더링 할 수 있도록 입력 어셈블러에서 정점 버퍼를 활성으로 설정합니다.
	Graphic->GetDeviceContext()->IASetVertexBuffers(0, 1, vertexBuffer.GetAddressOf(), &stride, &offset);

	// 렌더링 할 수 있도록 입력 어셈블러에서 인덱스 버퍼를 활성으로 설정합니다.
	Graphic->GetDeviceContext()->IASetIndexBuffer(indexBuffer.Get(), DXGI_FORMAT_R32_UINT, 0);

	// 이 버텍스 버퍼에서 렌더링되어야하는 프리미티브의 타입을 설정한다.이 경우 라인리스트이다.
	Graphic->GetDeviceContext()->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_LINELIST);

	Graphic->GetDeviceContext()->DrawIndexed(indexCount, 0, 0);
}

shared_ptr<TerrainBuffer> TerrainBuffer::Create(UINT _terrainWidth, UINT _terrainHeight)
{
	shared_ptr<TerrainBuffer> Instance(new TerrainBuffer());
	if (FAILED(Instance->Initialize(_terrainWidth, _terrainHeight)))
	{
		MSG_BOX("Failed to create TerrainBuffer.");
		return nullptr;
	}
	return Instance;
}
