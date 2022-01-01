#include "pch.h"
#include "HeightBrush.h"
#include "GraphicDevice.h"
#include "Transform.h"
#include "Shader.h"

HeightBrush::HeightBrush()
{
}

HeightBrush::HeightBrush(const HeightBrush & Rhs)
{
}

HeightBrush::~HeightBrush()
{
}

int HeightBrush::Update(float _TimeDelta)
{
	UpdateComponent(_TimeDelta);


	return 0;
}

void HeightBrush::Render()
{
	RenderComponent();
	RenderBuffer();
}

void HeightBrush::OnContact()
{
}

void HeightBrush::OnTrigger()
{
}

HRESULT HeightBrush::Initialize()
{
	Graphic = GraphicDevice::GetInstance();

	transform = Transform::Create(Transform::TRANSDESC(10.f, 10.f));
	transform->SetObject(shared_from_this());
	//transform->SetScale(Vector3(0.1f, 0.1f, 0.1f));


	D3D11_INPUT_ELEMENT_DESC InputLayout[] =
	{
		{ "POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 0, D3D11_INPUT_PER_VERTEX_DATA, 0 },
		{ "NORMAL", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_VERTEX_DATA, 0 },
		{ "TEXCOORD", 0, DXGI_FORMAT_R32G32_FLOAT, 0, D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_VERTEX_DATA, 0 }
	};

	shader = Shader::Create(InputLayout, sizeof(InputLayout), L"../Engine/ColorVS.hlsl", L"../Engine/ColorPS.hlsl");

	components[ComponentType::TRANSFORM] = transform;
	components[ComponentType::SHADER] = shader;

	if (!LoadHeightTexture())
		return E_FAIL;

	// 높이 맵의 높이를 표준화합니다.
	NormalizeHeightMap();

	// 지형 데이터의 법선을 계산합니다.
	if (!CalculateNormals())
		return E_FAIL;



	return InitializeBuffer();
}

bool HeightBrush::LoadHeightTexture()
{


	// 바이너리 모드로 높이맵 파일을 엽니다.
	FILE* filePtr = nullptr;
	if (fopen_s(&filePtr, "../Resources/Mountain2.bmp", "rb") != 0)
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
	heightMapWidth = (int)bitmapInfoHeader.biWidth;
	heightMapHeight = (int)bitmapInfoHeader.biHeight;

	// 비트맵 이미지 데이터의 크기를 계산합니다.
	int imageSize = heightMapWidth * heightMapHeight * 3;

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
	heightMap = new HeightMapType[heightMapHeight * heightMapWidth];
	if (!heightMap)
	{
		return false;
	}

	// 이미지 데이터 버퍼의 위치를 ​​초기화합니다.
	int k = 0;

	// 이미지 데이터를 높이 맵으로 읽어들입니다.
	for (int j = 0; j < heightMapHeight; j++)
	{
		for (int i = 0; i < heightMapWidth; i++)
		{
			unsigned char height = bitmapImage[k];

			int index = (heightMapHeight * j) + i;

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

void HeightBrush::NormalizeHeightMap()
{
	for (int j = 0; j < heightMapHeight; j++)
	{
		for (int i = 0; i < heightMapWidth; i++)
		{
			heightMap[(heightMapHeight * j) + i].y /= 15.0f;
		}
	}
}

bool HeightBrush::CalculateNormals()
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
	VectorType* normals = new VectorType[(heightMapHeight - 1) * (heightMapWidth - 1)];
	if (!normals)
	{
		return false;
	}

	// 메쉬의 모든면을 살펴보고 법선을 계산합니다.
	for (int j = 0; j < (heightMapHeight - 1); j++)
	{
		for (int i = 0; i < (heightMapWidth - 1); i++)
		{
			index1 = (j * heightMapHeight) + i;
			index2 = (j * heightMapHeight) + (i + 1);
			index3 = ((j + 1) * heightMapHeight) + i;

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

			index = (j * (heightMapHeight - 1)) + i;

			// 이 두 법선에 대한 정규화되지 않은 값을 얻기 위해 두 벡터의 외적을 계산합니다.
			normals[index].x = (vector1[1] * vector2[2]) - (vector1[2] * vector2[1]);
			normals[index].y = (vector1[2] * vector2[0]) - (vector1[0] * vector2[2]);
			normals[index].z = (vector1[0] * vector2[1]) - (vector1[1] * vector2[0]);
		}
	}

	// 이제 모든 정점을 살펴보고 각면의 평균을 취합니다. 	
	// 정점이 닿아 그 정점에 대한 평균 평균값을 얻는다.
	for (int j = 0; j < heightMapHeight; j++)
	{
		for (int i = 0; i < heightMapWidth; i++)
		{
			// 합계를 초기화합니다.
			sum[0] = 0.0f;
			sum[1] = 0.0f;
			sum[2] = 0.0f;

			// 카운트를 초기화합니다.
			count = 0;
			if (i - 1 >= 0)
				int a = 10;

			// 왼쪽 아래면.
			if (((i - 1) >= 0) && ((j - 1) >= 0))
			{
				index = ((j - 1) * (heightMapHeight - 1)) + (i - 1);

				sum[0] += normals[index].x;
				sum[1] += normals[index].y;
				sum[2] += normals[index].z;
				count++;
			}

			// 오른쪽 아래 면.
			if ((i < (heightMapWidth - 1)) && ((j - 1) >= 0))
			{
				index = ((j - 1) * (heightMapHeight - 1)) + i;

				sum[0] += normals[index].x;
				sum[1] += normals[index].y;
				sum[2] += normals[index].z;
				count++;
			}

			// 왼쪽 위 면.
			if (((i - 1) >= 0) && (j < (heightMapHeight - 1)))
			{
				index = (j * (heightMapHeight - 1)) + (i - 1);

				sum[0] += normals[index].x;
				sum[1] += normals[index].y;
				sum[2] += normals[index].z;
				count++;
			}

			// 오른쪽 위 면.
			if ((i < (heightMapWidth - 1)) && (j < (heightMapHeight - 1)))
			{
				index = (j * (heightMapHeight - 1)) + i;

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
			index = (j * heightMapHeight) + i;

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

HRESULT HeightBrush::InitializeBuffer()
{
	float tu = 0.0f;
	float tv = 0.0f;

	// 지형 메쉬의 정점 수를 계산합니다.
	vertexCount = (heightMapWidth - 1) * (heightMapHeight - 1) * 6;

	// 정점 배열을 만듭니다.
	vertices = new TerrainVertexType[vertexCount];
	if (!vertices)
	{
		return E_FAIL;
	}

	// 정점 배열에 대한 인덱스를 초기화합니다.
	int index = 0;

	// 지형 데이터로 정점 및 인덱스 배열을 로드합니다.
	for (int j = 0; j < (heightMapHeight - 1); j++)
	{
		for (int i = 0; i < (heightMapWidth - 1); i++)
		{
			int index1 = (heightMapHeight * j) + i;          // 왼쪽 아래.
			int index2 = (heightMapHeight * j) + (i + 1);      // 오른쪽 아래.
			int index3 = (heightMapHeight * (j + 1)) + i;      // 왼쪽 위.
			int index4 = (heightMapHeight * (j + 1)) + (i + 1);  // 오른쪽 위.

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


	// 정점, 인덱스 버퍼 생성.
	CreateStaticBuffer(GraphicDevice::GetInstance()->GetDevice(), vertices, vertexCount, sizeof(TerrainVertexType), D3D11_BIND_VERTEX_BUFFER, vertexBuffer.GetAddressOf());

	return S_OK;
}

void HeightBrush::RenderBuffer()
{
	// 정점 버퍼 보폭 및 오프셋을 설정합니다.
	unsigned int stride = sizeof(TerrainVertexType);
	unsigned int offset = 0;

	// 렌더링 할 수 있도록 입력 어셈블러에서 정점 버퍼를 활성으로 설정합니다.
	Graphic->GetDeviceContext()->IASetVertexBuffers(0, 1, vertexBuffer.GetAddressOf(), &stride, &offset);

	// 렌더링 할 수 있도록 입력 어셈블러에서 인덱스 버퍼를 활성으로 설정합니다.
	//Graphic->GetDeviceContext()->IASetIndexBuffer(indexBuffer.Get(), DXGI_FORMAT_R32_int, 0);

	// 이 버텍스 버퍼에서 렌더링되어야하는 프리미티브의 타입을 설정한다.이 경우 라인리스트이다.
	Graphic->GetDeviceContext()->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);

	Graphic->GetDeviceContext()->Draw(vertexCount, 0);
}

void HeightBrush::SetPosition(BrushDesc& _brushDesc)
{
	if (_brushDesc.position.x == 0 && _brushDesc.position.z == 0)
		return;

	Vector3 scale = Vector3(_brushDesc.range  * 0.1f);
	transform->SetScale(scale);

	brushWidth = (int)(heightMapWidth * transform->GetScale().x);
	brushHeight = (int)(heightMapHeight * transform->GetScale().z);


	Vector3 _pos;
	_pos.x = _brushDesc.position.x - brushWidth / 2;
	_pos.z = _brushDesc.position.z - brushHeight / 2;
	transform->SetState(Transform::POSITION, _pos);


}

float* HeightBrush::RaiseHeight(Vector3 * _pos)
{
	if (_pos[0] == EXCEPT_RAISEHEIGHT && _pos[1] == EXCEPT_RAISEHEIGHT && _pos[2] == EXCEPT_RAISEHEIGHT && _pos[3] == EXCEPT_RAISEHEIGHT)
		return nullptr;

	Ray ray[4] = {};

	ray[0].position = Vector3(_pos[0].x, 0.f, _pos[0].z);
	ray[0].direction = Vector3(0.f, 1.f, 0.f);

	ray[1].position = Vector3(_pos[1].x, 0.f, _pos[1].z);
	ray[1].direction = Vector3(0.f, 1.f, 0.f);

	ray[2].position = Vector3(_pos[2].x, 0.f, _pos[2].z);
	ray[2].direction = Vector3(0.f, 1.f, 0.f);

	ray[3].position = Vector3(_pos[3].x, 0.f, _pos[3].z);
	ray[3].direction = Vector3(0.f, 1.f, 0.f);



	float dist = 0.f;
	float distArray[4] = { 0 };



	for (UINT i = 0; i < vertexCount; i += 6)
	{

		Vector3 vertex1 = vertices[i + 0].position;
		Vector3 vertex2 = vertices[i + 1].position;
		Vector3 vertex3 = vertices[i + 2].position;
		Vector3 vertex4 = vertices[i + 5].position;

		if (vertex1.y == 0 && vertex2.y == 0 && vertex3.y == 0 && vertex4.y == 0)
			continue;

		Matrix worldMatrix = *transform->GetWorldMatrix();

		vertex1 = Vector3::Transform(vertex1, worldMatrix);
		vertex2 = Vector3::Transform(vertex2, worldMatrix);
		vertex3 = Vector3::Transform(vertex3, worldMatrix);
		vertex4 = Vector3::Transform(vertex4, worldMatrix);

		for (int j = 0; j < 4; ++j)
		{
			if (_pos[j] == EXCEPT_RAISEHEIGHT)
				continue;

			if (distArray[j] != 0)
				continue;

			if (ray[j].Intersects(vertex1, vertex2, vertex3, dist))
			{
				distArray[j] = dist * 100;
			}
			else if (ray[j].Intersects(vertex3, vertex2, vertex4, dist))
			{
				distArray[j] = dist * 100;
			}


			if (distArray[0] != 0 && distArray[1] != 0 && distArray[2] != 0 && distArray[3] != 0)
				return distArray;
		}
	}

	return distArray;
}

shared_ptr<HeightBrush> HeightBrush::Create()
{
	shared_ptr<HeightBrush> Instance(new HeightBrush());
	if (FAILED(Instance->Initialize()))
	{
		MSG_BOX("Failed to create HeightBrush.");
		return nullptr;
	}
	return Instance;
}
