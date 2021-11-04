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
	delete heightMap;
	heightMap = nullptr;
}

HRESULT TerrainBuffer::Initialize(UINT _terrainWidth, UINT _terrainHeight, wstring _heightMapPath)
{
	Graphic = GraphicDevice::GetInstance();

	terrainWidth = _terrainWidth;
	terrainHeight = _terrainHeight;

	if (_heightMapPath != L"")
	{
		if (FAILED(LoadHeightMap(_heightMapPath)))
		{
			MSG_BOX("Failed to load height map for TerrainBuffer.");
			return E_FAIL;
		}
		NormalizeHeightMap();
	}



	if (FAILED(InitializeBuffer()))
	{
		MSG_BOX("Failed to initial buffer for TerrainBuffer.");
		return E_FAIL;
	}


	return S_OK;
}

HRESULT TerrainBuffer::InitializeBuffer()
{
	// 지형 메쉬의 정점 수를 계산합니다.
	if (heightMap == nullptr)
		vertexCount = (terrainWidth - 1) * (terrainHeight - 1) * 8;
	else
		vertexCount = (terrainWidth - 1) * (terrainHeight - 1) * 12;


	// 인덱스 수를 꼭지점 수와 같게 설정합니다.
	indexCount = vertexCount;

	// 정점 배열을 만듭니다.
	VertexType* vertices = new VertexType[vertexCount];
	if (!vertices)
	{
		return false;
	}

	// 인덱스 배열을 만듭니다.
	ULONG* indices = new ULONG[indexCount];
	if (!indices)
		return E_FAIL;

	// 정점 배열에 대한 인덱스를 초기화합니다.
	int index = 0;

	// 지형 데이터로 정점 및 인덱스 배열을 로드합니다.
	for (UINT j = 0; j < (terrainHeight - 1); j++)
	{
		for (UINT i = 0; i < (terrainWidth - 1); i++)
		{

			// Get the indexes to the four points of the quad.
			int index1 = (terrainWidth * j) + i;          // Upper left.
			int index2 = (terrainWidth * j) + (i + 1);      // Upper right.
			int index3 = (terrainWidth * (j + 1)) + i;      // Bottom left.
			int index4 = (terrainWidth * (j + 1)) + (i + 1);  // Bottom right.

			// Now create two triangles for that quad.
			// Triangle 1 - Upper left.
			vertices[index].position.x = heightMap[index1].x;
			vertices[index].position.y = heightMap[index1].y;
			vertices[index].position.z = heightMap[index1].z;
			indices[index] = index;
			index++;

			// Triangle 1 - Upper right.
			vertices[index].position.x = heightMap[index2].x;
			vertices[index].position.y = heightMap[index2].y;
			vertices[index].position.z = heightMap[index2].z;
			indices[index] = index;
			index++;

			// Triangle 1 - Bottom left.
			vertices[index].position.x = heightMap[index3].x;
			vertices[index].position.y = heightMap[index3].y;
			vertices[index].position.z = heightMap[index3].z;
			indices[index] = index;
			index++;

			// Triangle 2 - Bottom left.
			vertices[index].position.x = heightMap[index3].x;
			vertices[index].position.y = heightMap[index3].y;
			vertices[index].position.z = heightMap[index3].z;
			indices[index] = index;
			index++;

			// Triangle 2 - Upper right.
			vertices[index].position.x = heightMap[index2].x;
			vertices[index].position.y = heightMap[index2].y;
			vertices[index].position.z = heightMap[index2].z;
			indices[index] = index;
			index++;

			// Triangle 2 - Bottom right.
			vertices[index].position.x = heightMap[index4].x;
			vertices[index].position.y = heightMap[index4].y;
			vertices[index].position.z = heightMap[index4].z;
			indices[index] = index;
			index++;

		}
	}

	if (FAILED(CreateStaticBuffer(Graphic->GetDevice(), vertices, vertexCount, sizeof(VertexType), D3D11_BIND_VERTEX_BUFFER, vertexBuffer.GetAddressOf())))
	{
		MSG_BOX("Failed to create vertex buffer for TerrainBuffer.");
		return E_FAIL;
	}

	if (FAILED(CreateStaticBuffer(Graphic->GetDevice(), indices, indexCount, sizeof(ULONG), D3D11_BIND_INDEX_BUFFER, indexBuffer.GetAddressOf())))
	{
		MSG_BOX("Failed to create index buffer for TerrainBuffer.");
		return E_FAIL;
	}


	// 이제 버퍼가 생성되고 로드된 배열을 해제하십시오.
	delete[] vertices;
	vertices = nullptr;

	delete[] indices;
	indices = nullptr;

	return S_OK;
}

HRESULT TerrainBuffer::LoadHeightMap(wstring _heightMapPath)
{
	// 바이너리 모드로 높이맵 파일을 엽니다.
	FILE* filePtr = nullptr;

	if (_wfopen_s(&filePtr, _heightMapPath.c_str(), L"rb") != 0)
		return E_FAIL;


	// 파일 헤더를 읽습니다.
	BITMAPFILEHEADER bitmapFileHeader;
	if (fread(&bitmapFileHeader, sizeof(BITMAPFILEHEADER), 1, filePtr) != 1)
		return E_FAIL;


	// 비트맵 정보 헤더를 읽습니다.
	BITMAPINFOHEADER bitmapInfoHeader;
	if (fread(&bitmapInfoHeader, sizeof(BITMAPINFOHEADER), 1, filePtr) != 1)
		return E_FAIL;

	// 지형의 크기를 저장합니다.
	terrainWidth = bitmapInfoHeader.biWidth;
	terrainHeight = bitmapInfoHeader.biHeight;

	// 비트맵 이미지 데이터의 크기를 계산합니다.
	int imageSize = terrainWidth * terrainHeight * 3;

	// 비트맵 이미지 데이터에 메모리를 할당합니다.
	unsigned char* bitmapImage = new unsigned char[imageSize];
	if (!bitmapImage)
		return E_FAIL;

	// 비트맵 데이터의 시작 부분으로 이동합니다.
	fseek(filePtr, bitmapFileHeader.bfOffBits, SEEK_SET);

	// 비트맵 이미지 데이터를 읽습니다.
	if (fread(bitmapImage, 1, imageSize, filePtr) != imageSize)
		return E_FAIL;

	// 파일을 닫습니다.
	if (fclose(filePtr) != 0)
		return E_FAIL;

	// 높이 맵 데이터를 저장할 구조체를 만듭니다.
	heightMap = new HeightMapType[terrainWidth * terrainHeight];
	if (!heightMap)
		return E_FAIL;

	// 이미지 데이터 버퍼의 위치를 ​​초기화합니다.
	int k = 0;

	// 이미지 데이터를 높이 맵으로 읽어들입니다.
	for (UINT j = 0; j < terrainHeight; j++)
	{
		for (UINT i = 0; i < terrainWidth; i++)
		{
			unsigned char height = bitmapImage[k];

			int index = (terrainHeight * j) + i;

			heightMap[index].x = (float)i;
			heightMap[index].y = (float)height;
			heightMap[index].z = (float)j;

			k += 3;
		}
	}

	// 비트맵 이미지 데이터를 해제합니다.
	delete[] bitmapImage;
	bitmapImage = nullptr;

	return S_OK;
}

void TerrainBuffer::NormalizeHeightMap()
{
	for (UINT j = 0; j < terrainHeight; j++)
	{
		for (UINT i = 0; i < terrainWidth; i++)
		{
			UINT index = index = (terrainWidth * j) + i;
			//heightMap[index].z += (float)(terrainHeight - 1);

			heightMap[index].y /= 15.0f;
		}
	}
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

void TerrainBuffer::Render()
{
	RenderBuffers();
}

shared_ptr<TerrainBuffer> TerrainBuffer::Create(UINT _terrainWidth, UINT _terrainHeight, wstring _heightMapPath)
{
	shared_ptr<TerrainBuffer> Instance(new TerrainBuffer());
	if (FAILED(Instance->Initialize(_terrainWidth, _terrainHeight, _heightMapPath)))
	{
		MSG_BOX("Failed to create TerrainBuffer.");
		return nullptr;
	}

	return Instance;
}
