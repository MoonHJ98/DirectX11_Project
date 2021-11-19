#include "pch.h"
#include "RectangleBuffer.h"
#include "GraphicDevice.h"

RectangleBuffer::RectangleBuffer()
{
}

RectangleBuffer::RectangleBuffer(const RectangleBuffer & Rhs)
{
}

RectangleBuffer::~RectangleBuffer()
{
}

HRESULT RectangleBuffer::Initialize()
{
	Graphic = GraphicDevice::GetInstance();


	return InitializeBuffer();
}

HRESULT RectangleBuffer::InitializeBuffer()
{
	D3D11_BUFFER_DESC vertexBufferDesc, indexBufferDesc;
	D3D11_SUBRESOURCE_DATA vertexData, indexData;

	// 정점 배열을 만듭니다.
	VertexType* vertices = new VertexType[vertexCount];
	if (!vertices)
		return E_FAIL;

	// 인덱스 배열을 만듭니다.
	ULONG* indices = new ULONG[indexCount];
	if (!indices)
		return E_FAIL;

	// 처음에는 정점 배열을 0으로 초기화합니다.
	memset(vertices, 0, (sizeof(VertexType) * vertexCount));


	// Set the number of vertices in the vertex array.
	vertexCount = 4;

	// Set the number of indices in the index array.
	indexCount = 6;

	// Create the vertex array.
	vertices = new VertexType[vertexCount];

	// Create the index array.
	indices = new ULONG[indexCount];

	// Load the vertex array with data.
	vertices[0].position = XMFLOAT3(-1.0f, -1.0f, 0.0f);  // Bottom left.
	vertices[0].texture = XMFLOAT2(0.0f, 1.0f);

	vertices[1].position = XMFLOAT3(1.0f, 1.0f, 0.0f);  // Top right.
	vertices[1].texture = XMFLOAT2(1.f, 0.f);

	vertices[2].position = XMFLOAT3(1.0f, -1.0f, 0.0f);  // Bottom right.
	vertices[2].texture = XMFLOAT2(1.0f, 1.0f);

	vertices[3].position = XMFLOAT3(-1.0f, 1.0f, 0.0f);  // Top left.
	vertices[3].texture = XMFLOAT2(0.f, 0.f);

	// Load the index array with data.
	indices[0] = 0;  // Bottom left.
	indices[1] = 1;  // Top left.
	indices[2] = 2;  // Bottom right.
	indices[3] = 0;	 // Bottom left.
	indices[4] = 3;  // Top left.
	indices[5] = 1;  // Top Right.

	// Set up the description of the static vertex buffer.
	vertexBufferDesc.Usage = D3D11_USAGE_DEFAULT;
	vertexBufferDesc.ByteWidth = sizeof(VertexType) * vertexCount;
	vertexBufferDesc.BindFlags = D3D11_BIND_VERTEX_BUFFER;
	vertexBufferDesc.CPUAccessFlags = 0;
	vertexBufferDesc.MiscFlags = 0;
	vertexBufferDesc.StructureByteStride = 0;

	// Give the subresource structure a pointer to the vertex data.
	vertexData.pSysMem = vertices;
	vertexData.SysMemPitch = 0;
	vertexData.SysMemSlicePitch = 0;

	// Now create the vertex buffer.
	if (FAILED(Graphic->GetDevice()->CreateBuffer(&vertexBufferDesc, &vertexData, vertexBuffer.GetAddressOf())))
	{
		MSG_BOX("Failed to create vertex buffer.");
		return E_FAIL;
	}

	// Set up the description of the static index buffer.
	indexBufferDesc.Usage = D3D11_USAGE_DEFAULT;
	indexBufferDesc.ByteWidth = sizeof(ULONG) * indexCount;
	indexBufferDesc.BindFlags = D3D11_BIND_INDEX_BUFFER;
	indexBufferDesc.CPUAccessFlags = 0;
	indexBufferDesc.MiscFlags = 0;
	indexBufferDesc.StructureByteStride = 0;

	// Give the subresource structure a pointer to the index data.
	indexData.pSysMem = indices;
	indexData.SysMemPitch = 0;
	indexData.SysMemSlicePitch = 0;

	// Create the index buffer.
	if (FAILED(Graphic->GetDevice()->CreateBuffer(&indexBufferDesc, &indexData, indexBuffer.GetAddressOf())))
	{
		MSG_BOX("Failed to create index buffer.");
		return E_FAIL;
	}

	// Release the arrays now that the vertex and index buffers have been created and loaded.
	delete[] vertices;
	vertices = nullptr;

	delete[] indices;
	indices = nullptr;

	return S_OK;
}

void RectangleBuffer::Render()
{
	RenderBuffer();
}

void RectangleBuffer::RenderBuffer()
{
	UINT stride;
	UINT offset;


	// Set vertex buffer stride and offset.
	stride = sizeof(VertexType);
	offset = 0;

	// Set the vertex buffer to active in the input assembler so it can be rendered.
	Graphic->GetDeviceContext()->IASetVertexBuffers(0, 1, vertexBuffer.GetAddressOf(), &stride, &offset);

	// Set the index buffer to active in the input assembler so it can be rendered.
	Graphic->GetDeviceContext()->IASetIndexBuffer(indexBuffer.Get(), DXGI_FORMAT_R32_UINT, 0);

	// Set the type of primitive that should be rendered from this vertex buffer, in this case triangles.
	Graphic->GetDeviceContext()->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);

	Graphic->GetDeviceContext()->DrawIndexed(indexCount, 0, 0);

}

shared_ptr<RectangleBuffer> RectangleBuffer::Create()
{
	shared_ptr<RectangleBuffer> Instance(new RectangleBuffer());
	if (FAILED(Instance->Initialize()))
	{
		MSG_BOX("Failed to create RectangleBuffer.");
		return nullptr;
	}
	return Instance;
}

int RectangleBuffer::Update(float _timeDelta)
{
	return 0;
}
