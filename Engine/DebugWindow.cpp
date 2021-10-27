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
	// 화면 크기를 저장하십시오.
	screenWidth = _screenWidth;
	screenHeight = _screenHeight;

	// 이 비트맵을 렌더링 할 픽셀의 크기를 저장합니다.
	bitmapWidth = _bitmapWidth;
	bitmapHeight = _bitmapHeight;

	// 정점 및 인덱스 버퍼를 초기화합니다.
	return InitializeBuffers();
}

void DebugWindow::Render()
{

	// 그리기를 준비하기 위해 그래픽 파이프 라인에 꼭지점과 인덱스 버퍼를 놓습니다.
	RenderBuffers();
}

int DebugWindow::GetIndexCount()
{
	return indexCount;
}

HRESULT DebugWindow::InitializeBuffers()
{
	// 정점 배열의 정점 수를 설정합니다.
	vertexCount = 6;

	// 인덱스 배열의 인덱스 수를 설정합니다.
	indexCount = vertexCount;

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

	// 데이터로 인덱스 배열을로드합니다.
	for (int i = 0; i < indexCount; i++)
	{
		indices[i] = i;
	}


	// 정적 정점 버퍼의 설명을 설정한다.
	D3D11_BUFFER_DESC vertexBufferDesc;
	vertexBufferDesc.Usage = D3D11_USAGE_DYNAMIC;
	vertexBufferDesc.ByteWidth = sizeof(VertexType) * vertexCount;
	vertexBufferDesc.BindFlags = D3D11_BIND_VERTEX_BUFFER;
	vertexBufferDesc.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
	vertexBufferDesc.MiscFlags = 0;
	vertexBufferDesc.StructureByteStride = 0;

	// subresource 구조에 정점 데이터에 대한 포인터를 제공합니다.
	D3D11_SUBRESOURCE_DATA vertexData;
	vertexData.pSysMem = vertices;
	vertexData.SysMemPitch = 0;
	vertexData.SysMemSlicePitch = 0;

	// 이제 정점 버퍼를 만듭니다.
	if (FAILED(Graphic->GetDevice()->CreateBuffer(&vertexBufferDesc, &vertexData, vertexBuffer.GetAddressOf())))
	{
		MSG_BOX("Faield to Create DebugWindow vertex buffer.");
		return E_FAIL;
	}

	// 정적 인덱스 버퍼의 설명을 설정합니다.
	D3D11_BUFFER_DESC indexBufferDesc;
	indexBufferDesc.Usage = D3D11_USAGE_DEFAULT;
	indexBufferDesc.ByteWidth = sizeof(ULONG) * indexCount;
	indexBufferDesc.BindFlags = D3D11_BIND_INDEX_BUFFER;
	indexBufferDesc.CPUAccessFlags = 0;
	indexBufferDesc.MiscFlags = 0;
	indexBufferDesc.StructureByteStride = 0;

	// 하위 리소스 구조에 인덱스 데이터에 대한 포인터를 제공합니다.
	D3D11_SUBRESOURCE_DATA indexData;
	indexData.pSysMem = indices;
	indexData.SysMemPitch = 0;
	indexData.SysMemSlicePitch = 0;

	// 인덱스 버퍼를 만듭니다.
	if (FAILED(Graphic->GetDevice()->CreateBuffer(&indexBufferDesc, &indexData, indexBuffer.GetAddressOf())))
	{
		MSG_BOX("Failed to create DebugWindow index buffer.");
		return E_FAIL;
	}

	// 비트 맵 왼쪽의 화면 좌표를 계산합니다.
	float left = (float)((screenWidth / 4) * -1);

	// 비트 맵 오른쪽의 화면 좌표를 계산합니다.
	float right = left + (float)bitmapWidth;

	// 비트 맵 상단의 화면 좌표를 계산합니다.
	float top = (float)(screenHeight / 4);

	// 비트 맵 아래쪽의 화면 좌표를 계산합니다.
	float bottom = top - (float)bitmapHeight;

	// 정점 배열에 데이터를로드합니다.
	// 첫 번째 삼각형.
	vertices[0].position = XMFLOAT3(left, top, 0.0f);  // Top left.
	vertices[0].texture = XMFLOAT2(0.0f, 0.0f);

	vertices[1].position = XMFLOAT3(right, bottom, 0.0f);  // Bottom right.
	vertices[1].texture = XMFLOAT2(1.0f, 1.0f);

	vertices[2].position = XMFLOAT3(left, bottom, 0.0f);  // Bottom left.
	vertices[2].texture = XMFLOAT2(0.0f, 1.0f);

	// 두 번째 삼각형.
	vertices[3].position = XMFLOAT3(left, top, 0.0f);  // Top left.
	vertices[3].texture = XMFLOAT2(0.0f, 0.0f);

	vertices[4].position = XMFLOAT3(right, top, 0.0f);  // Top right.
	vertices[4].texture = XMFLOAT2(1.0f, 0.0f);

	vertices[5].position = XMFLOAT3(right, bottom, 0.0f);  // Bottom right.
	vertices[5].texture = XMFLOAT2(1.0f, 1.0f);

	// 버텍스 버퍼를 쓸 수 있도록 잠급니다.
	D3D11_MAPPED_SUBRESOURCE mappedResource;
	if (FAILED(Graphic->GetDeviceContext()->Map(vertexBuffer.Get(), 0, D3D11_MAP_WRITE_DISCARD, 0, &mappedResource)))
	{
		return false;
	}

	// 정점 버퍼의 데이터를 가리키는 포인터를 얻는다.
	VertexType* verticesPtr = (VertexType*)mappedResource.pData;

	// 데이터를 정점 버퍼에 복사합니다.
	memcpy(verticesPtr, (void*)vertices, (sizeof(VertexType) * vertexCount));

	// 정점 버퍼의 잠금을 해제합니다.
	Graphic->GetDeviceContext()->Unmap(vertexBuffer.Get(), 0);

	// 이제 버텍스와 인덱스 버퍼가 생성되고로드 된 배열을 해제하십시오.
	delete[] vertices;
	vertices = nullptr;

	delete[] indices;
	indices = nullptr;

	return S_OK;
}

void DebugWindow::RenderBuffers()
{
	// 정점 버퍼 보폭 및 오프셋을 설정합니다.
	unsigned int stride = sizeof(VertexType);
	unsigned int offset = 0;

	// 렌더링 할 수 있도록 입력 어셈블러에서 정점 버퍼를 활성으로 설정합니다.
	Graphic->GetDeviceContext()->IASetVertexBuffers(0, 1, vertexBuffer.GetAddressOf(), &stride, &offset);

	// 렌더링 할 수 있도록 입력 어셈블러에서 인덱스 버퍼를 활성으로 설정합니다.
	Graphic->GetDeviceContext()->IASetIndexBuffer(indexBuffer.Get(), DXGI_FORMAT_R32_UINT, 0);

	// 이 꼭지점 버퍼에서 렌더링되어야하는 프리미티브 유형을 설정합니다.이 경우에는 삼각형입니다.
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
