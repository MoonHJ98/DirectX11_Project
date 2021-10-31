#include "pch.h"
#include "OrthoWindow.h"
#include "GraphicDevice.h"

OrthoWindow::OrthoWindow()
{
}

OrthoWindow::OrthoWindow(const OrthoWindow & Rhs)
{
}

OrthoWindow::~OrthoWindow()
{
}

HRESULT OrthoWindow::Initialize()
{
	Graphic = GraphicDevice::GetInstance();

	UINT num = 1;
	D3D11_VIEWPORT viewport;
	Graphic->GetDeviceContext()->RSGetViewports(&num, &viewport);

	return InitializeBuffers((int)viewport.Width, (int)viewport.Height);
}

HRESULT OrthoWindow::InitializeBuffers(int _windowWidth, int _windowHeight)
{
	// ������ ������ ȭ�� ��ǥ�� ����մϴ�.
	float left = (float)((_windowWidth / 2) * -1);

	// ������ �������� ȭ�� ��ǥ�� ����մϴ�.
	float right = left + (float)_windowWidth;

	// ������ ����� ȭ�� ��ǥ�� ����մϴ�.
	float top = (float)(_windowHeight / 2);

	// ������ �ϴ��� ȭ�� ��ǥ�� ����մϴ�.
	float bottom = top - (float)_windowHeight;

	// ���� �迭�� ���� ���� �����մϴ�.
	vertexCount = 6;

	// �ε��� �迭�� �ε��� ���� �����մϴ�.
	indexCount = vertexCount;

	// ���� �迭�� ����ϴ�.
	VertexType* vertices = new VertexType[vertexCount];
	if (!vertices)
	{
		return false;
	}

	// �ε��� �迭�� ����ϴ�.
	unsigned long* indices = new unsigned long[indexCount];
	if (!indices)
	{
		return false;
	}

	// ���� �迭�� �����͸��ε��մϴ�.
	// ù ��° �ﰢ��.
	vertices[0].position = XMFLOAT3(left, top, 0.0f);  // ���� ��
	vertices[0].texture = XMFLOAT2(0.0f, 0.0f);

	vertices[1].position = XMFLOAT3(right, bottom, 0.0f);  // ������ �Ʒ�
	vertices[1].texture = XMFLOAT2(1.0f, 1.0f);

	vertices[2].position = XMFLOAT3(left, bottom, 0.0f);  // ���� �Ʒ�
	vertices[2].texture = XMFLOAT2(0.0f, 1.0f);

	// �� ��° �ﰢ��.
	vertices[3].position = XMFLOAT3(left, top, 0.0f);  // ���� ��
	vertices[3].texture = XMFLOAT2(0.0f, 0.0f);

	vertices[4].position = XMFLOAT3(right, top, 0.0f);  // ������ ��
	vertices[4].texture = XMFLOAT2(1.0f, 0.0f);

	vertices[5].position = XMFLOAT3(right, bottom, 0.0f);  // ������ �Ʒ�
	vertices[5].texture = XMFLOAT2(1.0f, 1.0f);

	// �����ͷ� �ε��� �迭���ε��մϴ�.
	for (int i = 0; i < indexCount; i++)
	{
		indices[i] = i;
	}

	// ���� ���� ������ ����ü�� �����Ѵ�.
	D3D11_BUFFER_DESC vertexBufferDesc;
	vertexBufferDesc.Usage = D3D11_USAGE_DEFAULT;
	vertexBufferDesc.ByteWidth = sizeof(VertexType) * vertexCount;
	vertexBufferDesc.BindFlags = D3D11_BIND_VERTEX_BUFFER;
	vertexBufferDesc.CPUAccessFlags = 0;
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
		MSG_BOX("Failed to create OrthoWindow's vertex buffer.");
		return E_FAIL;
	}

	// ���� �ε��� ������ ����ü�� �����մϴ�.
	D3D11_BUFFER_DESC indexBufferDesc;
	indexBufferDesc.Usage = D3D11_USAGE_DEFAULT;
	indexBufferDesc.ByteWidth = sizeof(unsigned long) * indexCount;
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
		MSG_BOX("Failed to Create OrthoWindow's index buffer.");
		return E_FAIL;
	}

	// ���� ���ؽ��� �ε��� ���۰� �����ǰ�ε� �� �迭�� �����Ͻʽÿ�.
	delete[] vertices;
	vertices = 0;

	delete[] indices;
	indices = 0;

	return S_OK;
}

void OrthoWindow::RenderBuffers()
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

	Graphic->GetDeviceContext()->DrawIndexed(indexCount, 0, 0);

}

void OrthoWindow::Render()
{
	RenderBuffers();
}

int OrthoWindow::GetIndexCount()
{
	return 0;
}

shared_ptr<OrthoWindow> OrthoWindow::Create()
{
	shared_ptr<OrthoWindow> Instnace(new OrthoWindow());


	if (FAILED(Instnace->Initialize()))
	{
		MSG_BOX("Failed to create OrthoWindow.");
		return nullptr;
	}
	return Instnace;
}
