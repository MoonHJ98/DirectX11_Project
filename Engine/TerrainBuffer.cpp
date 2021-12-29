#include "pch.h"
#include "TerrainBuffer.h"
#include "GraphicDevice.h"
#include "Management.h"


TerrainBuffer::TerrainBuffer()
{
}

TerrainBuffer::TerrainBuffer(const TerrainBuffer & Rhs)
{
}

TerrainBuffer::~TerrainBuffer()
{
}

int TerrainBuffer::Update(float _timeDelta)
{
	timeDelta = _timeDelta;
	return 0;
}

void TerrainBuffer::Render()
{
	RenderBuffers();
}

void TerrainBuffer::RenderInspector()
{
}

HRESULT TerrainBuffer::Initialize(UINT _terrainWidth, UINT _terrainHeight, const char* heightMapFilename)
{
	Graphic = GraphicDevice::GetInstance();
	Manage = Management::GetInstance();

	terrainWidth = _terrainWidth;
	terrainHeight = _terrainHeight;

	UINT viewportNum = 1;
	D3D11_VIEWPORT viewport;
	Graphic->GetDeviceContext()->RSGetViewports(&viewportNum, &viewport);


	screenWidth = (UINT)viewport.Width;
	screenHeignt = (UINT)viewport.Height;

	shared_ptr<ConstantBuffer<BrushDesc>> temp(new ConstantBuffer<BrushDesc>());
	brushBuffer = temp;
	brushBuffer->Create(Graphic->GetDevice());

	// 지형의 높이 맵을 로드합니다.
	if (!LoadHeightMap(heightMapFilename))
		return E_FAIL;


	// 높이 맵의 높이를 표준화합니다.
	NormalizeHeightMap();

	// 지형 데이터의 법선을 계산합니다.
	if (!CalculateNormals())
		return E_FAIL;


	// 텍스처 좌표를 계산합니다.
	CalculateTextureCoordinates();

	return InitializeBuffers();
}

HRESULT TerrainBuffer::InitializeBuffers()
{

	float tu = 0.0f;
	float tv = 0.0f;

	// 지형 메쉬의 정점 수를 계산합니다.
	vertexCount = (terrainWidth - 1) * (terrainHeight - 1) * 6;

	// 정점 배열을 만듭니다.
	vertices = new TerrainVertexType[vertexCount];
	if (!vertices)
	{
		return E_FAIL;
	}

	// 정점 배열에 대한 인덱스를 초기화합니다.
	int index = 0;

	// 지형 데이터로 정점 및 인덱스 배열을 로드합니다.
	for (int j = 0; j < (terrainHeight - 1); j++)
	{
		for (int i = 0; i < (terrainWidth - 1); i++)
		{
			int index1 = (terrainHeight * j) + i;          // 왼쪽 아래.
			int index2 = (terrainHeight * j) + (i + 1);      // 오른쪽 아래.
			int index3 = (terrainHeight * (j + 1)) + i;      // 왼쪽 위.
			int index4 = (terrainHeight * (j + 1)) + (i + 1);  // 오른쪽 위.

			// 왼쪽 위.
			tv = heightMap[index3].tv;

			// 상단 가장자리를 덮도록 텍스처 좌표를 수정합니다.
			if (tv == 1.0f) { tv = 0.0f; }

			vertices[index].position = XMFLOAT3(heightMap[index3].x, heightMap[index3].y, heightMap[index3].z);
			vertices[index].Uv = XMFLOAT2(heightMap[index3].tu, tv);
			vertices[index].normal = XMFLOAT3(heightMap[index3].nx, heightMap[index3].ny, heightMap[index3].nz);
			index++;

			// 오른쪽 위.
			tu = heightMap[index4].tu;
			tv = heightMap[index4].tv;

			// 위쪽과 오른쪽 가장자리를 덮도록 텍스처 좌표를 수정합니다.
			if (tu == 0.0f) { tu = 1.0f; }
			if (tv == 1.0f) { tv = 0.0f; }

			vertices[index].position = XMFLOAT3(heightMap[index4].x, heightMap[index4].y, heightMap[index4].z);
			vertices[index].Uv = XMFLOAT2(tu, tv);
			vertices[index].normal = XMFLOAT3(heightMap[index4].nx, heightMap[index4].ny, heightMap[index4].nz);
			index++;

			// 왼쪽 아래.
			vertices[index].position = XMFLOAT3(heightMap[index1].x, heightMap[index1].y, heightMap[index1].z);
			vertices[index].Uv = XMFLOAT2(heightMap[index1].tu, heightMap[index1].tv);
			vertices[index].normal = XMFLOAT3(heightMap[index1].nx, heightMap[index1].ny, heightMap[index1].nz);
			index++;

			// 왼쪽 아래.
			vertices[index].position = XMFLOAT3(heightMap[index1].x, heightMap[index1].y, heightMap[index1].z);
			vertices[index].Uv = XMFLOAT2(heightMap[index1].tu, heightMap[index1].tv);
			vertices[index].normal = XMFLOAT3(heightMap[index1].nx, heightMap[index1].ny, heightMap[index1].nz);
			index++;

			// 오른쪽 위.
			tu = heightMap[index4].tu;
			tv = heightMap[index4].tv;

			// 위쪽과 오른쪽 가장자리를 덮도록 텍스처 좌표를 수정합니다.
			if (tu == 0.0f) { tu = 1.0f; }
			if (tv == 1.0f) { tv = 0.0f; }

			vertices[index].position = XMFLOAT3(heightMap[index4].x, heightMap[index4].y, heightMap[index4].z);
			vertices[index].Uv = XMFLOAT2(tu, tv);
			vertices[index].normal = XMFLOAT3(heightMap[index4].nx, heightMap[index4].ny, heightMap[index4].nz);
			index++;

			// 오른쪽 아래.
			tu = heightMap[index2].tu;

			// 오른쪽 가장자리를 덮도록 텍스처 좌표를 수정합니다.
			if (tu == 0.0f) { tu = 1.0f; }

			vertices[index].position = XMFLOAT3(heightMap[index2].x, heightMap[index2].y, heightMap[index2].z);
			vertices[index].Uv = XMFLOAT2(tu, heightMap[index2].tv);
			vertices[index].normal = XMFLOAT3(heightMap[index2].nx, heightMap[index2].ny, heightMap[index2].nz);
			index++;
		}
	}


	//// 정점, 인덱스 버퍼 생성.
	//CreateStaticBuffer(GraphicDevice::GetInstance()->GetDevice(), vertices, vertexCount, sizeof(TerrainVertexType), D3D11_BIND_VERTEX_BUFFER, vertexBuffer.GetAddressOf());

	D3D11_BUFFER_DESC desc;
	desc.Usage = D3D11_USAGE_DYNAMIC;
	desc.ByteWidth = sizeof(TerrainVertexType) * vertexCount;
	desc.BindFlags = D3D11_BIND_VERTEX_BUFFER;
	desc.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
	desc.MiscFlags = 0;
	desc.StructureByteStride = 0;

	D3D11_SUBRESOURCE_DATA data;
	data.pSysMem = vertices;
	data.SysMemPitch = 0;
	data.SysMemSlicePitch = 0;

	Graphic->GetDevice()->CreateBuffer(&desc, &data, vertexBuffer.GetAddressOf());
	return S_OK;
}


Vector3 TerrainBuffer::PickTerrain(Vector2 screenPos)
{
	//POINT p;
	//GetCursorPos(&p);
	//ScreenToClient(hWnd, &p);


	Matrix	matProj;
	matProj = *Manage->GetTransform(D3DTRANSFORMSTATE_PROJECTION);


	// 뷰포트의 마우스를 투영의 마우스로 변환 -> 뷰스페이스로 변환
	Vector3		Temp;

	Temp.x = (float(screenPos.x) / (screenWidth >> 1) - 1.f) / matProj._11;
	Temp.y = (float(-screenPos.y) / (screenHeignt >> 1) + 1.f) / matProj._22;
	Temp.z = 1.f;

	// 뷰 스페이스 상의 rayPos, rayDir
	Vector3 RayPos = Vector3(0.f, 0.f, 0.f);
	Vector3 RayDir = Temp - RayPos;
	RayDir.Normalize();

	// 월드로 변환
	Matrix	matView;
	matView = *Manage->GetTransform(D3DTRANSFORMSTATE_VIEW);
	//Matrix world = *transform->GetWorldMatrix();
	//Matrix WorldView = world * matView;
	//Matrix invmatrix = WorldView.Invert();
	matView = matView.Invert();


	Vector3::Transform(RayPos, matView, RayPos);
	Vector3::TransformNormal(RayDir, matView, RayDir);



	Ray ray;

	float dist = 0.f;


	Vector3 Pos;

	int index = 0;
	for (int i = 0; i < vertexCount; i += 6)
	{
		ray.position = RayPos;

		ray.direction = RayDir;

		Vector3 vertex1 = vertices[i + 0].position;
		Vector3 vertex2 = vertices[i + 1].position;
		Vector3 vertex3 = vertices[i + 2].position;
		Vector3 vertex4 = vertices[i + 3].position;
		Vector3 vertex5 = vertices[i + 4].position;
		Vector3 vertex6 = vertices[i + 5].position;

		if (ray.Intersects(vertex1, vertex2, vertex3, dist))
		{
			Pos = ray.position + ray.direction * dist;
			brushDesc.position = Pos;
			brushBuffer->SetData(Graphic->GetDeviceContext(), brushDesc);
			auto buffer = brushBuffer->GetBuffer();
			Graphic->GetDeviceContext()->PSSetConstantBuffers(0, 1, &buffer);
			return Pos;
		}
		if (ray.Intersects(vertex4, vertex5, vertex6, dist))
		{
			Pos = ray.position + ray.direction * dist;
			brushDesc.position = Pos;
			brushBuffer->SetData(Graphic->GetDeviceContext(), brushDesc);
			auto buffer = brushBuffer->GetBuffer();
			Graphic->GetDeviceContext()->PSSetConstantBuffers(0, 1, &buffer);
			return Pos;
		}
	}
	brushDesc.position = Pos;
	brushBuffer->SetData(Graphic->GetDeviceContext(), brushDesc);
	auto buffer = brushBuffer->GetBuffer();
	Graphic->GetDeviceContext()->PSSetConstantBuffers(0, 1, &buffer);

	return Pos;
}

int TerrainBuffer::GetVertexCount()
{
	return vertexCount;
}

int TerrainBuffer::GetIndexCount()
{
	return indexCount;
}

void TerrainBuffer::CopyVertexArray(void * vertexList)
{
	memcpy(vertexList, vertices, sizeof(TerrainVertexType) * vertexCount);

}

void TerrainBuffer::CopyIndexArray(void * indexList)
{
	memcpy(indexList, indices, sizeof(UINT) * indexCount);

}

void TerrainBuffer::RaiseHeight()
{
	D3D11_RECT rect;

	rect.left = (LONG)(brushDesc.position.x - brushDesc.range);
	rect.top = (LONG)(brushDesc.position.z + brushDesc.range);
	rect.right = (LONG)(brushDesc.position.x + brushDesc.range);
	rect.bottom = (LONG)(brushDesc.position.z - brushDesc.range);

	if (rect.left < 0)
		rect.left = 0;
	if (rect.top >= terrainHeight)
		rect.top = (LONG)terrainHeight;
	if (rect.right >= (LONG)terrainWidth)
		rect.right = (LONG)terrainWidth;
	if (rect.bottom < 0)
		rect.bottom = 0;

	float dist;

	for (int i = 0; i < vertexCount; i+=6)
	{
		for (int j = 0; j < 6; ++j)
		{
			Vector3 vertex = vertices[i + j].position;
			float dx = vertex.x - brushDesc.position.x;
			float dz = vertex.z - brushDesc.position.z;

			// 원의 중심과 vertex 사이의 거리구하기 (피타고라스 정리)
			dist = sqrt(dx * dx + dz * dz);

			// 원의 중심과 정점의 거리가 반지름의 길이를 넘어갈 경우 계산 안함.
			if (fabsf(dist) > brushDesc.range)
				continue;

			/*
			원의 중심에서 반지름의 길이를 넘어가는 dist(거리)는 위에서 걸러지고
			dist의 값은 정점이 원이 중심과 멀수록 range의 값과 동일해지고 가까워질수록 0으로 간다.
			위를 생각하면서 아래 공식에 값을 넣어보면 어떤 결과가 나오는지 알 수 있다.
			*/

			//float h = brushDesc.range - dist; // 원뿔 형태가 나옴.
			float h = (float)(pow(brushDesc.range, 2) - (dist * dist)); // 반원 형태가 나옴.

			if (Manage->GetDIKeyState(DIK_LALT) & 0x80)
				vertices[i + j].position.y -= h * timeDelta;
			else
				vertices[i + j].position.y += h * timeDelta;
		}
	}


	D3D11_MAPPED_SUBRESOURCE mappedResource;
	if (SUCCEEDED(Graphic->GetDeviceContext()->Map(vertexBuffer.Get(), 0, D3D11_MAP_WRITE_DISCARD, 0, &mappedResource)))
	{
		memcpy(mappedResource.pData, vertices, sizeof(TerrainVertexType) * vertexCount);
		Graphic->GetDeviceContext()->Unmap(vertexBuffer.Get(), 0);
	}

	CreateNormalData();
}

void TerrainBuffer::RenderBuffers()
{
	// 정점 버퍼 보폭 및 오프셋을 설정합니다.
	unsigned int stride = sizeof(TerrainVertexType);
	unsigned int offset = 0;

	// 렌더링 할 수 있도록 입력 어셈블러에서 정점 버퍼를 활성으로 설정합니다.
	Graphic->GetDeviceContext()->IASetVertexBuffers(0, 1, vertexBuffer.GetAddressOf(), &stride, &offset);

	// 렌더링 할 수 있도록 입력 어셈블러에서 인덱스 버퍼를 활성으로 설정합니다.
	Graphic->GetDeviceContext()->IASetIndexBuffer(indexBuffer.Get(), DXGI_FORMAT_R32_UINT, 0);

	// 이 버텍스 버퍼에서 렌더링되어야하는 프리미티브의 타입을 설정한다.이 경우 라인리스트이다.
	Graphic->GetDeviceContext()->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);

	Graphic->GetDeviceContext()->Draw(vertexCount, 0);
	//Graphic->GetDeviceContext()->DrawIndexed(indexCount, 0, 0);
}

bool TerrainBuffer::LoadHeightMap(const char * heightMapFilename)
{
	if (heightMapFilename == nullptr)
	{
		heightMap = new HeightMapType[terrainWidth * terrainHeight];

		for (int j = 0; j < terrainHeight; j++)
		{
			for (int i = 0; i < terrainWidth; i++)
			{

				int index = (terrainHeight * j) + i;

				heightMap[index].x = (float)i;
				heightMap[index].y = (float)0.f;
				heightMap[index].z = (float)j;
			}
		}
		return true;
	}
	// 바이너리 모드로 높이맵 파일을 엽니다.
	FILE* filePtr = nullptr;
	if (fopen_s(&filePtr, heightMapFilename, "rb") != 0)
	{
		return false;
	}

	// 파일 헤더를 읽습니다.
	BITMAPFILEHEADER bitmapFileHeader;
	if (fread(&bitmapFileHeader, sizeof(BITMAPFILEHEADER), 1, filePtr) != 1)
	{
		return false;
	}

	// 비트맵 정보 헤더를 읽습니다.
	BITMAPINFOHEADER bitmapInfoHeader;
	if (fread(&bitmapInfoHeader, sizeof(BITMAPINFOHEADER), 1, filePtr) != 1)
	{
		return false;
	}

	// 지형의 크기를 저장합니다.
	terrainWidth = bitmapInfoHeader.biWidth;
	terrainHeight = bitmapInfoHeader.biHeight;

	// 비트맵 이미지 데이터의 크기를 계산합니다.
	int imageSize = terrainWidth * terrainHeight * 3;

	// 비트맵 이미지 데이터에 메모리를 할당합니다.
	unsigned char* bitmapImage = new unsigned char[imageSize];
	if (!bitmapImage)
	{
		return false;
	}

	// 비트맵 데이터의 시작 부분으로 이동합니다.
	fseek(filePtr, bitmapFileHeader.bfOffBits, SEEK_SET);

	// 비트맵 이미지 데이터를 읽습니다.
	if (fread(bitmapImage, 1, imageSize, filePtr) != imageSize)
	{
		return false;
	}

	// 파일을 닫습니다.
	if (fclose(filePtr) != 0)
	{
		return false;
	}

	// 높이 맵 데이터를 저장할 구조체를 만듭니다.
	heightMap = new HeightMapType[terrainWidth * terrainHeight];
	if (!heightMap)
	{
		return false;
	}

	// 이미지 데이터 버퍼의 위치를 ​​초기화합니다.
	int k = 0;

	// 이미지 데이터를 높이 맵으로 읽어들입니다.
	for (int j = 0; j < terrainHeight; j++)
	{
		for (int i = 0; i < terrainWidth; i++)
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
	bitmapImage = 0;

	return true;
}

void TerrainBuffer::NormalizeHeightMap()
{
	for (int j = 0; j < terrainHeight; j++)
	{
		for (int i = 0; i < terrainWidth; i++)
		{
			heightMap[(terrainHeight * j) + i].y /= 15.0f;
		}
	}
}

bool TerrainBuffer::CalculateNormals()
{
	int index1 = 0;
	int index2 = 0;
	int index3 = 0;
	int index = 0;
	int count = 0;
	float vertex1[3] = { 0.f, 0.f, 0.f };
	float vertex2[3] = { 0.f, 0.f, 0.f };
	float vertex3[3] = { 0.f, 0.f, 0.f };
	float vector1[3] = { 0.f, 0.f, 0.f };
	float vector2[3] = { 0.f, 0.f, 0.f };
	float sum[3] = { 0.f, 0.f, 0.f };
	float length = 0.0f;


	// 정규화되지 않은 법선 벡터를 저장할 임시 배열을 만듭니다.
	VectorType* normals = new VectorType[(terrainHeight - 1) * (terrainWidth - 1)];
	if (!normals)
	{
		return false;
	}

	// 메쉬의 모든면을 살펴보고 법선을 계산합니다.
	for (int j = 0; j < (terrainHeight - 1); j++)
	{
		for (int i = 0; i < (terrainWidth - 1); i++)
		{
			index1 = (j * terrainHeight) + i;
			index2 = (j * terrainHeight) + (i + 1);
			index3 = ((j + 1) * terrainHeight) + i;

			// 표면에서 세 개의 꼭지점을 가져옵니다.
			vertex1[0] = heightMap[index1].x;
			vertex1[1] = heightMap[index1].y;
			vertex1[2] = heightMap[index1].z;

			vertex2[0] = heightMap[index2].x;
			vertex2[1] = heightMap[index2].y;
			vertex2[2] = heightMap[index2].z;

			vertex3[0] = heightMap[index3].x;
			vertex3[1] = heightMap[index3].y;
			vertex3[2] = heightMap[index3].z;

			// 표면의 두 벡터를 계산합니다.
			vector1[0] = vertex1[0] - vertex3[0];
			vector1[1] = vertex1[1] - vertex3[1];
			vector1[2] = vertex1[2] - vertex3[2];
			vector2[0] = vertex3[0] - vertex2[0];
			vector2[1] = vertex3[1] - vertex2[1];
			vector2[2] = vertex3[2] - vertex2[2];

			index = (j * (terrainHeight - 1)) + i;

			// 이 두 법선에 대한 정규화되지 않은 값을 얻기 위해 두 벡터의 외적을 계산합니다.
			normals[index].x = (vector1[1] * vector2[2]) - (vector1[2] * vector2[1]);
			normals[index].y = (vector1[2] * vector2[0]) - (vector1[0] * vector2[2]);
			normals[index].z = (vector1[0] * vector2[1]) - (vector1[1] * vector2[0]);
		}
	}

	// 이제 모든 정점을 살펴보고 각면의 평균을 취합니다. 	
	// 정점이 닿아 그 정점에 대한 평균 평균값을 얻는다.
	for (int j = 0; j < terrainHeight; j++)
	{
		for (int i = 0; i < terrainWidth; i++)
		{
			// 합계를 초기화합니다.
			sum[0] = 0.0f;
			sum[1] = 0.0f;
			sum[2] = 0.0f;

			// 카운트를 초기화합니다.
			count = 0;

			// 왼쪽 아래면.
			if (((i - 1) >= 0) && ((j - 1) >= 0))
			{
				index = ((j - 1) * (terrainHeight - 1)) + (i - 1);

				sum[0] += normals[index].x;
				sum[1] += normals[index].y;
				sum[2] += normals[index].z;
				count++;
			}

			// 오른쪽 아래 면.
			if ((i < (terrainWidth - 1)) && ((j - 1) >= 0))
			{
				index = ((j - 1) * (terrainHeight - 1)) + i;

				sum[0] += normals[index].x;
				sum[1] += normals[index].y;
				sum[2] += normals[index].z;
				count++;
			}

			// 왼쪽 위 면.
			if (((i - 1) >= 0) && (j < (terrainHeight - 1)))
			{
				index = (j * (terrainHeight - 1)) + (i - 1);

				sum[0] += normals[index].x;
				sum[1] += normals[index].y;
				sum[2] += normals[index].z;
				count++;
			}

			// 오른쪽 위 면.
			if ((i < (terrainWidth - 1)) && (j < (terrainHeight - 1)))
			{
				index = (j * (terrainHeight - 1)) + i;

				sum[0] += normals[index].x;
				sum[1] += normals[index].y;
				sum[2] += normals[index].z;
				count++;
			}

			// 이 정점에 닿는면의 평균을 취합니다.
			sum[0] = (sum[0] / (float)count);
			sum[1] = (sum[1] / (float)count);
			sum[2] = (sum[2] / (float)count);

			// 이 법선의 길이를 계산합니다.
			length = (float)sqrt((sum[0] * sum[0]) + (sum[1] * sum[1]) + (sum[2] * sum[2]));

			// 높이 맵 배열의 정점 위치에 대한 인덱스를 가져옵니다.
			index = (j * terrainHeight) + i;

			// 이 정점의 최종 공유 법선을 표준화하여 높이 맵 배열에 저장합니다.
			heightMap[index].nx = (sum[0] / length);
			heightMap[index].ny = (sum[1] / length);
			heightMap[index].nz = (sum[2] / length);
		}
	}

	// 임시 법선을 해제합니다.
	delete[] normals;
	normals = 0;

	return true;
}

void TerrainBuffer::CalculateTextureCoordinates()
{
	// 텍스처 좌표를 얼마나 많이 증가 시킬지 계산합니다.
	float incrementValue = (float)TEXTURE_REPEAT / (float)terrainWidth;

	// 텍스처를 반복 할 횟수를 계산합니다.
	int incrementCount = terrainWidth / TEXTURE_REPEAT;

	// tu 및 tv 좌표 값을 초기화합니다.
	float tuCoordinate = 0.0f;
	float tvCoordinate = 1.0f;

	//  tu 및 tv 좌표 인덱스를 초기화합니다.
	int tuCount = 0;
	int tvCount = 0;

	// 전체 높이 맵을 반복하고 각 꼭지점의 tu 및 tv 텍스처 좌표를 계산합니다.
	for (int j = 0; j < terrainHeight; j++)
	{
		for (int i = 0; i < terrainWidth; i++)
		{
			// 높이 맵에 텍스처 좌표를 저장한다.
			heightMap[(terrainHeight * j) + i].tu = tuCoordinate;
			heightMap[(terrainHeight * j) + i].tv = tvCoordinate;

			// tu 텍스처 좌표를 증가 값만큼 증가시키고 인덱스를 1 씩 증가시킨다.
			tuCoordinate += incrementValue;
			tuCount++;

			// 텍스처의 오른쪽 끝에 있는지 확인하고, 그렇다면 처음부터 다시 시작하십시오.
			if (tuCount == incrementCount)
			{
				tuCoordinate = 0.0f;
				tuCount = 0;
			}
		}

		// tv 텍스처 좌표를 증가 값만큼 증가시키고 인덱스를 1 씩 증가시킵니다.
		tvCoordinate -= incrementValue;
		tvCount++;

		// 텍스처의 상단에 있는지 확인하고, 그렇다면 하단에서 다시 시작합니다.
		if (tvCount == incrementCount)
		{
			tvCoordinate = 1.0f;
			tvCount = 0;
		}
	}
}

void TerrainBuffer::CreateNormalData()
{
	for (int i = 0; i < vertexCount; i+=6)
	{
		auto vertex1 = vertices[i + 0];
		auto vertex2 = vertices[i + 1];
		auto vertex3 = vertices[i + 2];
		auto vertex4 = vertices[i + 3];
		auto vertex5 = vertices[i + 4];
		auto vertex6 = vertices[i + 5];


		Vector3 d1 = vertex2.position - vertex1.position;
		Vector3 d2 = vertex3.position - vertex1.position;

		Vector3 d3 = vertex4.position - vertex6.position;
		Vector3 d4 = vertex5.position - vertex6.position;


		Vector3 normal, normal2;

		normal = XMVector3Cross(d1, d2);
		normal2 = XMVector3Cross(d3, d4);


		vertices[i + 0].normal = XMVector3Normalize(vertices[i + 0].normal += normal);
		vertices[i + 1].normal = XMVector3Normalize(vertices[i + 1].normal += normal);
		vertices[i + 2].normal = XMVector3Normalize(vertices[i + 2].normal += normal);

		vertices[i + 3].normal = XMVector3Normalize(vertices[i + 3].normal += normal2);
		vertices[i + 4].normal = XMVector3Normalize(vertices[i + 4].normal += normal2);
		vertices[i + 5].normal = XMVector3Normalize(vertices[i + 5].normal += normal2);
	}

}

shared_ptr<TerrainBuffer> TerrainBuffer::Create(UINT _terrainWidth, UINT _terrainHeight, const char* heightMapFilename)
{
	shared_ptr<TerrainBuffer> Instance(new TerrainBuffer());
	if (FAILED(Instance->Initialize(_terrainWidth, _terrainHeight, heightMapFilename)))
	{
		MSG_BOX("Failed to create TerrainBuffer.");
		return nullptr;
	}
	return Instance;
}
