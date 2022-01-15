#include "pch.h"
#include "TerrainBuffer.h"
#include "GraphicDevice.h"
#include "Management.h"
#include "HeightBrush.h"
#include "Texture.h"
#include "PhysXManager.h"
#include "TextureBrush.h"
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


	textureInfoBufferDesc.useAlpha = TRUE;
	textureInfoBuffer->SetData(Graphic->GetDeviceContext(), textureInfoBufferDesc);
	auto buffer2 = textureInfoBuffer->GetBuffer();
	Graphic->GetDeviceContext()->PSSetConstantBuffers(3, 1, &buffer2);


	return 0;
}

void TerrainBuffer::Render()
{
	textureBrush->Render();
	Graphic->GetDeviceContext()->PSSetShaderResources(2, 1, textures[0]->GetTexture());
	Graphic->GetDeviceContext()->PSSetShaderResources(3, 1, textures[1]->GetTexture());
	Graphic->GetDeviceContext()->PSSetShaderResources(4, 1, textures[2]->GetTexture());



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
	PickTerrain(Vector2());
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

		switch (terrainToolStyle)
		{
		case TerrainToolStyle::RaiseOrLowerTerrain:
			ImGui::Image((void*)heightMapTexture.Get(), ImVec2(125, 125));
			break;
		case TerrainToolStyle::PaintTexture:
			ImGui::Image((void*)selectedTextureForPaint.Get(), ImVec2(125, 125));
			break;
		default:
			break;
		}

		ImGui::SameLine();


		switch (terrainToolStyle)
		{
		case TerrainToolStyle::RaiseOrLowerTerrain:
			HeightMapSelect();
			break;
		case TerrainToolStyle::PaintTexture:
			TextureForPaintSelect();
			break;
		default:
			break;
		}



	}
	//else
	//	terrainTool = FALSE;

	if (Manage->GetDIMouseState(InputDevice::DIM_LB) & 0x80)
	{
		switch (terrainToolStyle)
		{
		case TerrainBuffer::TerrainToolStyle::RaiseOrLowerTerrain:
			RaiseHeight();
			break;
		case TerrainBuffer::TerrainToolStyle::PaintTexture:
			DrawAlphaMap();
			break;
		case TerrainBuffer::TerrainToolStyle::TerrainToolStyleEnd:
			break;
		default:
			break;
		}
	}

}

HRESULT TerrainBuffer::Initialize(UINT _terrainWidth, UINT _terrainHeight, HWND _hWnd, const char* heightMapFilename)
{
	hWnd = _hWnd;
	Graphic = GraphicDevice::GetInstance();
	Manage = Management::GetInstance();

	terrainWidth = _terrainWidth;
	terrainHeight = _terrainHeight;

	brushDesc.terrainHeight = _terrainHeight;
	brushDesc.terrainWidth = _terrainWidth;


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

	shared_ptr<ConstantBuffer<TextureInfoBufferDesc>> temp3(new ConstantBuffer<TextureInfoBufferDesc>());
	textureInfoBuffer = temp3;
	textureInfoBuffer->Create(Graphic->GetDevice());


	// Brushes for raise Height.
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

	// Sub Texture for splatting
	TEXTUREDESC desc;
	desc.MaterialName = L"Terrain0";
	desc.Path = L"../Resources/Terrain0.tga";
	textures.push_back(Texture::Create(Graphic->GetDevice(), &desc));

	desc.MaterialName = L"Terrain1";
	desc.Path = L"../Resources/Terrain1.tga";
	textures.push_back(Texture::Create(Graphic->GetDevice(), &desc));

	desc.MaterialName = L"Terrain2";
	desc.Path = L"../Resources/Terrain2.tga";
	textures.push_back(Texture::Create(Graphic->GetDevice(), &desc));

	selectedTextureForPaint = *textures[0]->GetTexture();

	textureBrush = TextureBrush::Create();

	return InitializeBuffers();
}

HRESULT TerrainBuffer::InitializeBuffers()
{
	vertexCount = terrainWidth * terrainHeight;
	vertices = new TerrainVertexType[vertexCount];

	for (int z = 0; z < terrainHeight; ++z)
	{
		for (int x = 0; x < terrainWidth; ++x)
		{

			UINT index = terrainWidth * z + x;
			vertices[index].position = Vector3((float)x, 0.f, (float)z);
			vertices[index].normal = Vector3(0.f, 1.f, 0.f);
			vertices[index].Uv = Vector2((float)x / (float)terrainWidth, (float)(terrainHeight - 1 - z) / (float)terrainHeight);

		}
	}


	for (int z = 0; z < terrainHeight - 1; ++z)
	{
		for (int x = 0; x < terrainWidth - 1; ++x)
		{
			indices.push_back(terrainWidth * z + x);
			indices.push_back(terrainWidth * (z + 1) + x);
			indices.push_back(terrainWidth * z + x + 1);

			indices.push_back(terrainWidth * z + x + 1);
			indices.push_back(terrainWidth * (z + 1) + x);
			indices.push_back(terrainWidth * (z + 1) + x + 1);
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
	CreateStaticBuffer(Graphic->GetDevice(), &indices[0], indices.size(), sizeof(UINT), D3D11_BIND_INDEX_BUFFER, indexBuffer.GetAddressOf());
	indexCount = indices.size();

	return S_OK;
}


Vector3 TerrainBuffer::PickTerrain(Vector2 screenPos)
{
	POINT p;
	GetCursorPos(&p);
	ScreenToClient(hWnd, &p);


	Matrix	matProj;
	matProj = *Manage->GetTransform(D3DTRANSFORMSTATE_PROJECTION);

	//cout << p.x << " , " << p.y<< endl;

	// 뷰포트의 마우스를 투영의 마우스로 변환 -> 뷰스페이스로 변환
	Vector3		Temp;

	Temp.x = (float(p.x) / (screenWidth >> 1) - 1.f) / matProj._11;
	Temp.y = (float(-p.y) / (screenHeignt >> 1) + 1.f) / matProj._22;
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
				brushDesc.position = Pos;
				brushBuffer->SetData(Graphic->GetDeviceContext(), brushDesc);
				auto buffer = brushBuffer->GetBuffer();
				Graphic->GetDeviceContext()->PSSetConstantBuffers(0, 1, &buffer);

				//cout << Pos.x << " , " << Pos.y << " , " << Pos.z << endl;
				if (terrainToolStyle == TerrainToolStyle::RaiseOrLowerTerrain)
				{
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
				}
				return Pos;
			}
			if (ray.Intersects(vertex4, vertex2, vertex3, dist))
			{
				Pos = ray.position + ray.direction * dist;
				brushDesc.position = Pos;
				brushBuffer->SetData(Graphic->GetDeviceContext(), brushDesc);
				auto buffer = brushBuffer->GetBuffer();
				Graphic->GetDeviceContext()->PSSetConstantBuffers(0, 1, &buffer);

				//cout << Pos.x << " , " << Pos.y << " , " << Pos.z << endl;

				if (terrainToolStyle == TerrainToolStyle::RaiseOrLowerTerrain)
				{
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
				}

				return Pos;
			}
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
	//memcpy(indexList, indices, sizeof(UINT) * indexCount);

}

void TerrainBuffer::RaiseHeight()
{
	if (heightMapOption == HeigitMapOptionEnd)
		return;

	for (UINT i = 0; i < indexCount; i += 6)
	{
		Vector3 vertex1 = vertices[indices[i + 0]].position;
		Vector3 vertex2 = vertices[indices[i + 1]].position;
		Vector3 vertex3 = vertices[indices[i + 2]].position;
		Vector3 vertex4 = vertices[indices[i + 5]].position;


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
				vertices[indices[i + 0]].position.y -= h1 * timeDelta * 0.5f;
				vertices[indices[i + 1]].position.y -= h2 * timeDelta * 0.5f;
				vertices[indices[i + 2]].position.y -= h3 * timeDelta * 0.5f;

				vertices[indices[i + 3]].position.y -= h3 * timeDelta * 0.5f;
				vertices[indices[i + 4]].position.y -= h2 * timeDelta * 0.5f;
				vertices[indices[i + 5]].position.y -= h4 * timeDelta * 0.5f;

				for (int j = 0; j < 6; ++j)
				{
					if (vertices[indices[i + j]].position.y <= 0.f)
						vertices[indices[i + j]].position.y = 0.f;
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
				vertices[indices[i + 0]].position.y += h1 * timeDelta * 0.5f;
				vertices[indices[i + 1]].position.y += h2 * timeDelta * 0.5f;
				vertices[indices[i + 2]].position.y += h3 * timeDelta * 0.5f;

				vertices[indices[i + 3]].position.y += h3 * timeDelta * 0.5f;
				vertices[indices[i + 4]].position.y += h2 * timeDelta * 0.5f;
				vertices[indices[i + 5]].position.y += h4 * timeDelta * 0.5f;
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

	//Graphic->GetDeviceContext()->Draw(vertexCount, 0);
	Graphic->GetDeviceContext()->DrawIndexed(indexCount, 0, 0);
}


void TerrainBuffer::CreateNormalData()
{
	for (UINT i = 0; i < indexCount / 3; ++i)
	{
		auto index0 = indices[i * 3 + 0];
		auto index1 = indices[i * 3 + 1];
		auto index2 = indices[i * 3 + 2];

		auto vertex0 = vertices[index0];
		auto vertex1 = vertices[index1];
		auto vertex2 = vertices[index2];

		Vector3 d1 = vertex1.position - vertex0.position;
		Vector3 d2 = vertex2.position - vertex0.position;

		Vector3 normal;
		normal = XMVector3Cross(d1, d2);

		vertices[index0].normal = XMVector3Normalize(vertices[index0].normal + normal);
		vertices[index1].normal = XMVector3Normalize(vertices[index1].normal + normal);
		vertices[index2].normal = XMVector3Normalize(vertices[index2].normal + normal);


	}
}

void TerrainBuffer::HeightMapSelect()
{
	if (ImGui::BeginTable("HeightMapSelect", heightMapTextureCount, ImGuiTableFlags_BordersOuter, ImVec2(50.f * heightMapTextureCount, 125.f)))
	{
		if (changeTerrainToolRender == true)
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

void TerrainBuffer::TextureForPaintSelect()
{

	for (UINT i = 0; i < textures.size(); i++)
	{
		if (changeTerrainToolRender == true)
			terrainTool = TRUE;

		ImGui::PushID(i);
		if ((i % 3) != 0)
			ImGui::SameLine();

		if (ImGui::ImageButton(*(textures[i])->GetTexture(), ImVec2(32, 32)))
		{
			selectedTextureForPaint = *(textures[i])->GetTexture();
			textureOption = i;
		}

		// Our buttons are both drag sources and drag targets here!
		if (ImGui::BeginDragDropSource(ImGuiDragDropFlags_None))
		{
			// Set payload to carry the index of our item (could be anything)
			ImGui::SetDragDropPayload("DND_DEMO_CELL", &i, sizeof(int));

			// Display preview (could be anything, e.g. when dragging an image we could decide to display
			// the filename and a small preview of the image, etc.)
			//if (mode == Mode_Copy) 
			//	{ ImGui::Text("Copy %s", names[i]); }
			//if (mode == Mode_Move) 
			//	{ ImGui::Text("Move %s", names[i]); }
			//if (mode == Mode_Swap)

			ImGui::Text("Swap %s", ToString(textures[i]->GetTextureDecs().MaterialName).c_str());
			ImGui::EndDragDropSource();
		}

		if (ImGui::BeginDragDropTarget())
		{
			if (const ImGuiPayload* payload = ImGui::AcceptDragDropPayload("DND_DEMO_CELL"))
			{
				int a = 10;
				IM_ASSERT(payload->DataSize == sizeof(int));
				int payload_n = *(const int*)payload->Data;
				{
					auto temp = textures[i];
					textures[i] = textures[payload_n];
					textures[payload_n] = temp;
				}
			}
			ImGui::EndDragDropTarget();

		}

		ImGui::PopID();
	}

}


void TerrainBuffer::DrawAlphaMap()
{
	textureBrush->DrawAlphaMap(brushDesc, textureOption);
}



shared_ptr<TerrainBuffer> TerrainBuffer::Create(UINT _terrainWidth, UINT _terrainHeight, HWND _hWnd, const char* heightMapFilename)
{
	shared_ptr<TerrainBuffer> Instance(new TerrainBuffer());
	if (FAILED(Instance->Initialize(_terrainWidth, _terrainHeight, _hWnd, heightMapFilename)))
	{
		MSG_BOX("Failed to create TerrainBuffer.");
		return nullptr;
	}
	return Instance;
}
