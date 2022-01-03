#include "pch.h"
#include "TerrainBuffer.h"
#include "GraphicDevice.h"
#include "Management.h"
#include "HeightBrush.h"
#include "Texture.h"


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

	toolDesc._terrainTool = terrainTool;
	toolBuffer->SetData(Graphic->GetDeviceContext(), toolDesc);
	auto buffer = toolBuffer->GetBuffer();
	Graphic->GetDeviceContext()->PSSetConstantBuffers(2, 1, &buffer);

	return 0;
}

void TerrainBuffer::Render()
{
	RenderBuffers();

	switch (heightMapOption)
	{
	case TerrainBuffer::Circle:
		break;
	case TerrainBuffer::Mountain:
		mountain->Render();
		break;
	case TerrainBuffer::DesertMountain:
		desertMountain->Render();
		break;
	case TerrainBuffer::HeigitMapOptionEnd:
		break;
	default:
		break;
	}
	
}

void TerrainBuffer::RenderInspector()
{
	static char RaiseOrLowerTerrain[1024 * 16] =
		" Left click to raise.\n"
		" Hold shift and left click to lower.\n";

	static char PaintTexture[1024 * 16] = 
		"Paints the selected material layor onto the terrain texture.";
	

	if (ImGui::CollapsingHeader("Terrain"))
	{

		ImGui::Spacing();
		ImGui::Spacing();

		ImGui::Combo("Terrain Setting", &terrainToolStyle, "Raise or Lower Terrain\0Paint Texture\0");

		ImGui::Spacing();
		ImGui::Spacing();

		if (terrainToolStyle == TerrainToolStyle::RaiseOrLowerTerrain)
			ImGui::InputTextMultiline("##source0", RaiseOrLowerTerrain, IM_ARRAYSIZE(RaiseOrLowerTerrain), ImVec2(-FLT_MIN, ImGui::GetTextLineHeight() * 4), ImGuiInputTextFlags_ReadOnly);
		if (terrainToolStyle == TerrainToolStyle::PaintTexture)
			ImGui::InputTextMultiline("##source1", PaintTexture, IM_ARRAYSIZE(PaintTexture), ImVec2(-FLT_MIN, ImGui::GetTextLineHeight() * 4), ImGuiInputTextFlags_ReadOnly);

		ImGui::Spacing();
		ImGui::Spacing();

		ImGui::Text("Brushes");

		ImGui::Spacing();


		ImGui::Image((void*)heightMapTexture.Get(), ImVec2(125, 125));

		ImGui::SameLine();



		if (ImGui::BeginTable("HeightMapSelect", heightMapTextureCount, ImGuiTableFlags_BordersOuter, ImVec2(50.f * heightMapTextureCount, 125.f)))
		{
			if(changeTerrainToolRender == true)
				terrainTool = TRUE;

			ImGui::TableNextRow();

			ImGui::TableSetColumnIndex(0);
			if (ImGui::ImageButton(*circleTexture->GetTexture(), ImVec2(32, 32)))
			{
				heightMapTexture = *circleTexture->GetTexture();
				heightMapOption = Circle;
			}

			ImGui::TableSetColumnIndex(1);
			if (ImGui::ImageButton(*mountainTexture->GetTexture(), ImVec2(32, 32)))
			{
				heightMapTexture = *mountainTexture->GetTexture();
				heightMapOption = Mountain;
			}

			ImGui::TableSetColumnIndex(2);
			if (ImGui::ImageButton(*desertMountainTexture->GetTexture(), ImVec2(32, 32)))
			{
				heightMapTexture = *desertMountainTexture->GetTexture();
				heightMapOption = DesertMountain;
			}

			ImGui::EndTable();
		}


	}
	else
		terrainTool = FALSE;

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

	shared_ptr<ConstantBuffer<TerrainToolDesc>> temp2(new ConstantBuffer<TerrainToolDesc>());
	toolBuffer = temp2;
	toolBuffer->Create(Graphic->GetDevice());

	mountain = HeightBrush::Create("../Resources/Mountain2.bmp");
	TEXTUREDESC mountainDesc;
	mountainDesc.Path = L"../Resources/Mountain2.png";
	mountainTexture = Texture::Create(Graphic->GetDevice(), &mountainDesc);
	++heightMapTextureCount;

	desertMountain = HeightBrush::Create("../Resources/Desert-Mountain-1.bmp");
	TEXTUREDESC desertMountainDesc;
	desertMountainDesc.Path = L"../Resources/Desert-Mountain-1.png";
	desertMountainTexture = Texture::Create(Graphic->GetDevice(), &desertMountainDesc);
	++heightMapTextureCount;

	TEXTUREDESC circleDesc;
	circleDesc.Path = L"../Resources/Circle.png";
	circleTexture = Texture::Create(Graphic->GetDevice(), &circleDesc);
	++heightMapTextureCount;

	heightMapTexture = *circleTexture->GetTexture();

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
	for (UINT i = 0; i < vertexCount; i += 6)
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

			switch (heightMapOption)
			{
			case TerrainBuffer::Circle:
				break;
			case TerrainBuffer::Mountain:
				mountain->SetPosition(brushDesc);
				break;
			case TerrainBuffer::DesertMountain:
				desertMountain->SetPosition(brushDesc);
				break;
			case TerrainBuffer::HeigitMapOptionEnd:
				break;
			default:
				break;
			}

			//cout << Pos.x << " , " << Pos.z << endl;
			return Pos;
		}
		else if (ray.Intersects(vertex4, vertex5, vertex6, dist))
		{
			Pos = ray.position + ray.direction * dist;


			brushDesc.position = Pos;
			brushBuffer->SetData(Graphic->GetDeviceContext(), brushDesc);
			auto buffer = brushBuffer->GetBuffer();
			Graphic->GetDeviceContext()->PSSetConstantBuffers(0, 1, &buffer);

			switch (heightMapOption)
			{
			case TerrainBuffer::Circle:
				break;
			case TerrainBuffer::Mountain:
				mountain->SetPosition(brushDesc);
				break;
			case TerrainBuffer::DesertMountain:
				desertMountain->SetPosition(brushDesc);
				break;
			case TerrainBuffer::HeigitMapOptionEnd:
				break;
			default:
				break;
			}

			//cout << Pos.x << " , " << Pos.z << endl;

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
	if (heightMapOption == HeigitMapOptionEnd)
		return;

	for (UINT i = 0; i < vertexCount; i += 6)
	{
		Vector3 vertex1 = vertices[i + 0].position;
		Vector3 vertex2 = vertices[i + 1].position;
		Vector3 vertex3 = vertices[i + 2].position;
		Vector3 vertex4 = vertices[i + 5].position;


		float dx1 = vertex1.x - brushDesc.position.x;
		float dz1 = vertex1.z - brushDesc.position.z;

		float dx2 = vertex2.x - brushDesc.position.x;
		float dz2 = vertex2.z - brushDesc.position.z;

		float dx3 = vertex3.x - brushDesc.position.x;
		float dz3 = vertex3.z - brushDesc.position.z;

		float dx4 = vertex4.x - brushDesc.position.x;
		float dz4 = vertex4.z - brushDesc.position.z;

		float dist1 = sqrt(dx1 * dx1 + dz1 * dz1);
		float dist2 = sqrt(dx2 * dx2 + dz2 * dz2);
		float dist3 = sqrt(dx3 * dx3 + dz3 * dz3);
		float dist4 = sqrt(dx4 * dx4 + dz4 * dz4);

		float h1 = 0.f;
		float h2 = 0.f;
		float h3 = 0.f;
		float h4 = 0.f;

		if (fabsf(dist1) > brushDesc.range)
			vertex1 = EXCEPT_RAISEHEIGHT;
		if (fabsf(dist2) > brushDesc.range)
			vertex2 = EXCEPT_RAISEHEIGHT;
		if (fabsf(dist3) > brushDesc.range)
			vertex3 = EXCEPT_RAISEHEIGHT;
		if (fabsf(dist4) > brushDesc.range)
			vertex4 = EXCEPT_RAISEHEIGHT;

		if (vertex1 == EXCEPT_RAISEHEIGHT && vertex2 == EXCEPT_RAISEHEIGHT && vertex3 == EXCEPT_RAISEHEIGHT && vertex4 == EXCEPT_RAISEHEIGHT)
			continue;

		Vector3 pos[4] = { vertex1, vertex2, vertex3, vertex4 };

		float* distArray = nullptr;

		switch (heightMapOption)
		{
		case TerrainBuffer::Circle:
			break;
		case TerrainBuffer::Mountain:
			distArray = mountain->RaiseHeight(pos);
			break;
		case TerrainBuffer::DesertMountain:
			distArray = desertMountain->RaiseHeight(pos);
			break;
		case TerrainBuffer::HeigitMapOptionEnd:
			break;
		default:
			break;
		}


		if (heightMapOption != Circle && distArray == nullptr)
			continue;

		
		if (distArray != nullptr)
		{
			h1 = distArray[0];
			h2 = distArray[1];
			h3 = distArray[2];
			h4 = distArray[3];
		}

		if (heightMapOption == Circle)
		{
			h1 = (float)(pow(brushDesc.range, 2) - (dist1 * dist1));
			if (h1 <= 0)
				h1 = 0;
			h2 = (float)(pow(brushDesc.range, 2) - (dist2 * dist2));
			if (h2 <= 0)
				h2 = 0;
			h3 = (float)(pow(brushDesc.range, 2) - (dist3 * dist3));
			if (h3 <= 0)
				h3 = 0;
			h4 = (float)(pow(brushDesc.range, 2) - (dist4 * dist4));
			if (h4 <= 0)
				h4 = 0;

		}

		//h1 = (float)(pow(brushDesc.range, 2) - (dist1 * dist1)); // 반원 형태가 나옴.

		if (Manage->GetDIKeyState(DIK_LSHIFT) & 0x80)
		{
			switch (terrainToolStyle)
			{
			case TerrainBuffer::RaiseOrLowerTerrain:
				vertices[i + 0].position.y -= h1 * timeDelta;
				vertices[i + 1].position.y -= h2 * timeDelta;
				vertices[i + 2].position.y -= h3 * timeDelta;

				vertices[i + 3].position.y -= h3 * timeDelta;
				vertices[i + 4].position.y -= h2 * timeDelta;
				vertices[i + 5].position.y -= h4 * timeDelta;

				for (int j = 0; j < 6; ++j)
				{
					if (vertices[i + j].position.y <= 0.f)
						vertices[i + j].position.y = 0.f;
				}
				break;
			case TerrainBuffer::PaintTexture:
				break;
			case TerrainBuffer::TerrainToolStyleEnd:
				break;
			default:
				break;
			}
		}
		else
		{
			switch (terrainToolStyle)
			{
			case TerrainBuffer::RaiseOrLowerTerrain:
				vertices[i + 0].position.y += h1 * timeDelta;
				vertices[i + 1].position.y += h2 * timeDelta;
				vertices[i + 2].position.y += h3 * timeDelta;

				vertices[i + 3].position.y += h3 * timeDelta;
				vertices[i + 4].position.y += h2 * timeDelta;
				vertices[i + 5].position.y += h4 * timeDelta;
				break;
			case TerrainBuffer::PaintTexture:
				break;
			case TerrainBuffer::TerrainToolStyleEnd:
				break;
			default:
				break;
			}
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

void TerrainBuffer::SetScreenSize(Vector2 _sceneSize)
{
	screenWidth = (UINT)_sceneSize.x;
	screenHeignt = (UINT)_sceneSize.y;
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
	for (UINT i = 0; i < vertexCount; i += 6)
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
