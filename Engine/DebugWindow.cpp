#include "pch.h"
#include "DebugWindow.h"
#include "GraphicDevice.h"

DebugWindow::DebugWindow()
{
}

DebugWindow::DebugWindow(const DebugWindow & Rhs)
{
}

DebugWindow::~DebugWindow()
{
}

HRESULT DebugWindow::Initialize(int _screenWidth, int _screenHeight, int _bitmapWidth, int _bitmapHeight)
{
	Graphic = GraphicDevice::GetInstance();
	// ȭ�� ũ�⸦ �����Ͻʽÿ�.
	screenWidth = _screenWidth;
	screenHeight = _screenHeight;

	// �� ��Ʈ���� ������ �� �ȼ��� ũ�⸦ �����մϴ�.
	bitmapWidth = _bitmapWidth;
	bitmapHeight = _bitmapHeight;

	// ���� �� �ε��� ���۸� �ʱ�ȭ�մϴ�.
	return InitializeBuffers();
}

void DebugWindow::Render()
{

	// �׸��⸦ �غ��ϱ� ���� �׷��� ������ ���ο� �������� �ε��� ���۸� �����ϴ�.
	RenderBuffers();
}

int DebugWindow::GetIndexCount()
{
	return indexCount;
}

HRESULT DebugWindow::InitializeBuffers()
{
	// ���� �迭�� ���� ���� �����մϴ�.
	vertexCount = 6;

	// �ε��� �迭�� �ε��� ���� �����մϴ�.
	indexCount = vertexCount;

	// ���� �迭�� ����ϴ�.
	VertexType* vertices = new VertexType[vertexCount];
	if (!vertices)
		return E_FAIL;

	// �ε��� �迭�� ����ϴ�.
	ULONG* indices = new ULONG[indexCount];
	if (!indices)
		return E_FAIL;

	// ó������ ���� �迭�� 0���� �ʱ�ȭ�մϴ�.
	memset(vertices, 0, (sizeof(VertexType) * vertexCount));

	// �����ͷ� �ε��� �迭���ε��մϴ�.
	for (int i = 0; i < indexCount; i++)
	{
		indices[i] = i;
	}


	// ���� ���� ������ ������ �����Ѵ�.
	D3D11_BUFFER_DESC vertexBufferDesc;
	vertexBufferDesc.Usage = D3D11_USAGE_DYNAMIC;
	vertexBufferDesc.ByteWidth = sizeof(VertexType) * vertexCount;
	vertexBufferDesc.BindFlags = D3D11_BIND_VERTEX_BUFFER;
	vertexBufferDesc.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
	vertexBufferDesc.MiscFlags = 0;
	vertexBufferDesc.StructureByteStride = 0;

	// subresource ������ ���� �����Ϳ� ���� �����͸� �����մϴ�.
	D3D11_SUBRESOURCE_DATA vertexData;
	vertexData.pSysMem = vertices;
	vertexData.SysMemPitch = 0;
	vertexData.SysMemSlicePitch = 0;

	// ���� ���� ���۸� ����ϴ�.
	if (FAILED(Graphic->GetDevice()->CreateBuffer(&vertexBufferDesc, &vertexData, vertexBuffer.GetAddressOf())))
	{
		MSG_BOX("Faield to Create DebugWindow vertex buffer.");
		return E_FAIL;
	}

	// ���� �ε��� ������ ������ �����մϴ�.
	D3D11_BUFFER_DESC indexBufferDesc;
	indexBufferDesc.Usage = D3D11_USAGE_DEFAULT;
	indexBufferDesc.ByteWidth = sizeof(ULONG) * indexCount;
	indexBufferDesc.BindFlags = D3D11_BIND_INDEX_BUFFER;
	indexBufferDesc.CPUAccessFlags = 0;
	indexBufferDesc.MiscFlags = 0;
	indexBufferDesc.StructureByteStride = 0;

	// ���� ���ҽ� ������ �ε��� �����Ϳ� ���� �����͸� �����մϴ�.
	D3D11_SUBRESOURCE_DATA indexData;
	indexData.pSysMem = indices;
	indexData.SysMemPitch = 0;
	indexData.SysMemSlicePitch = 0;

	// �ε��� ���۸� ����ϴ�.
	if (FAILED(Graphic->GetDevice()->CreateBuffer(&indexBufferDesc, &indexData, indexBuffer.GetAddressOf())))
	{
		MSG_BOX("Failed to create DebugWindow index buffer.");
		return E_FAIL;
	}

	// ��Ʈ �� ������ ȭ�� ��ǥ�� ����մϴ�.
	float left = (float)((screenWidth / 4) * -1);

	// ��Ʈ �� �������� ȭ�� ��ǥ�� ����մϴ�.
	float right = left + (float)bitmapWidth;

	// ��Ʈ �� ����� ȭ�� ��ǥ�� ����մϴ�.
	float top = (float)(screenHeight / 4);

	// ��Ʈ �� �Ʒ����� ȭ�� ��ǥ�� ����մϴ�.
	float bottom = top - (float)bitmapHeight;

	// ���� �迭�� �����͸��ε��մϴ�.
	// ù ��° �ﰢ��.
	vertices[0].position = XMFLOAT3(left, top, 0.0f);  // Top left.
	vertices[0].texture = XMFLOAT2(0.0f, 0.0f);

	vertices[1].position = XMFLOAT3(right, bottom, 0.0f);  // Bottom right.
	vertices[1].texture = XMFLOAT2(1.0f, 1.0f);

	vertices[2].position = XMFLOAT3(left, bottom, 0.0f);  // Bottom left.
	vertices[2].texture = XMFLOAT2(0.0f, 1.0f);

	// �� ��° �ﰢ��.
	vertices[3].position = XMFLOAT3(left, top, 0.0f);  // Top left.
	vertices[3].texture = XMFLOAT2(0.0f, 0.0f);

	vertices[4].position = XMFLOAT3(right, top, 0.0f);  // Top right.
	vertices[4].texture = XMFLOAT2(1.0f, 0.0f);

	vertices[5].position = XMFLOAT3(right, bottom, 0.0f);  // Bottom right.
	vertices[5].texture = XMFLOAT2(1.0f, 1.0f);

	// ���ؽ� ���۸� �� �� �ֵ��� ��޴ϴ�.
	D3D11_MAPPED_SUBRESOURCE mappedResource;
	if (FAILED(Graphic->GetDeviceContext()->Map(vertexBuffer.Get(), 0, D3D11_MAP_WRITE_DISCARD, 0, &mappedResource)))
	{
		return false;
	}

	// ���� ������ �����͸� ����Ű�� �����͸� ��´�.
	VertexType* verticesPtr = (VertexType*)mappedResource.pData;

	// �����͸� ���� ���ۿ� �����մϴ�.
	memcpy(verticesPtr, (void*)vertices, (sizeof(VertexType) * vertexCount));

	// ���� ������ ����� �����մϴ�.
	Graphic->GetDeviceContext()->Unmap(vertexBuffer.Get(), 0);

	// ���� ���ؽ��� �ε��� ���۰� �����ǰ�ε� �� �迭�� �����Ͻʽÿ�.
	delete[] vertices;
	vertices = nullptr;

	delete[] indices;
	indices = nullptr;

	return S_OK;
}

void DebugWindow::RenderBuffers()
{
	// ���� ���� ���� �� �������� �����մϴ�.
	unsigned int stride = sizeof(VertexType);
	unsigned int offset = 0;

	// ������ �� �� �ֵ��� �Է� ��������� ���� ���۸� Ȱ������ �����մϴ�.
	Graphic->GetDeviceContext()->IASetVertexBuffers(0, 1, vertexBuffer.GetAddressOf(), &stride, &offset);

	// ������ �� �� �ֵ��� �Է� ��������� �ε��� ���۸� Ȱ������ �����մϴ�.
	Graphic->GetDeviceContext()->IASetIndexBuffer(indexBuffer.Get(), DXGI_FORMAT_R32_UINT, 0);

	// �� ������ ���ۿ��� �������Ǿ���ϴ� ������Ƽ�� ������ �����մϴ�.�� ��쿡�� �ﰢ���Դϴ�.
	Graphic->GetDeviceContext()->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
}

shared_ptr<DebugWindow> DebugWindow::Create(int _screenWidth, int _screenHeight, int _bitmapWidth, int _bitmapHeight)
{
	shared_ptr<DebugWindow> Instance(new DebugWindow());
	if (FAILED(Instance->Initialize(_screenWidth, _screenHeight, _bitmapWidth, _bitmapHeight)))
	{
		MSG_BOX("Failed to create DebugWindow.");
		return nullptr;
	}
	return Instance;
}
