#include "pch.h"
#include "TerrainBuffer.h"
#include "Management.h"
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

int TerrainBuffer::Update(float _timeDelta)
{
	return 0;
}

void TerrainBuffer::Render()
{
}

void TerrainBuffer::RenderInspector()
{
}

int TerrainBuffer::GetVertexCount()
{
	return vertexCount;
}

void TerrainBuffer::CopyVertexArray(void * vertexList)
{
	memcpy(vertexList, vertices, sizeof(TerrainVertexType) * vertexCount);
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

	for (int z = 0; z < terrainHeight - 1; ++z)
	{
		for (int x = 0; x < terrainWidth - 1; ++x)
		{
			int	index[4] = { -1 };
			index[0] = terrainWidth * z + x;
			index[1] = terrainWidth * (z + 1) + x;
			index[2] = terrainWidth * z + x + 1;
			index[3] = terrainWidth * (z + 1) + (x + 1);


			Vector3 vertex1 = vertices[index[0]].position;
			Vector3 vertex2 = vertices[index[1]].position;
			Vector3 vertex3 = vertices[index[2]].position;
			Vector3 vertex4 = vertices[index[3]].position;

			ray.position = RayPos;

			ray.direction = RayDir;


			if (ray.Intersects(vertex1, vertex2, vertex3, dist))
			{
				Pos = ray.position + ray.direction * dist;
				return Pos;
			}
			if (ray.Intersects(vertex4, vertex2, vertex3, dist))
			{
				Pos = ray.position + ray.direction * dist;
				return Pos;
			}
		}
	}

	//brushDesc.position = Pos;
	//brushBuffer->SetData(Graphic->GetDeviceContext(), brushDesc);
	//auto buffer = brushBuffer->GetBuffer();
	//Graphic->GetDeviceContext()->PSSetConstantBuffers(0, 1, &buffer);

	//cout << Pos.x << " , " << Pos.y << " , " << Pos.z << endl;
	return Vector3(0.f, 0.f, 0.f);
}

bool TerrainBuffer::LoadHeightMap(const char * heightMapFilename)
{
	if (heightMapFilename == nullptr)
	{
		terrainHeight = 250;
		terrainWidth = 250;
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

	return S_OK;
}

HRESULT TerrainBuffer::Initialize(const char * heightMapFilename)
{
	Manage = Management::GetInstance();
	Graphic = GraphicDevice::GetInstance();

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

	// 지형에 대한 지오 메트릭을 포함하는 정점 및 인덱스 버퍼를 초기화합니다.
	return InitializeBuffers();

}

shared_ptr<TerrainBuffer> TerrainBuffer::Create(const char * heightMapFilename)
{
	shared_ptr<TerrainBuffer> Instance(new TerrainBuffer());
	if (FAILED(Instance->Initialize(heightMapFilename)))
	{
		MSG_BOX("Failed to create TerrainBuffer.");
		return nullptr;
	}
	return Instance;
}
