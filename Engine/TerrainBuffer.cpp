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

HRESULT TerrainBuffer::Initialize(HWND _hWnd, UINT _terrainWidth, UINT _terrainHeight)
{
	Graphic = GraphicDevice::GetInstance();
	Manage = Management::GetInstance();
	hWnd = _hWnd;
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

	return InitializeBuffers();
}

HRESULT TerrainBuffer::InitializeBuffers()
{
	vector<VertexType> v;


	vertexCount = terrainWidth * terrainHeight;
	vertices = new VertexType[vertexCount];


	for (UINT z = 0; z < terrainHeight; ++z)
	{
		for (UINT x = 0; x < terrainWidth; ++x)
		{

			UINT index = terrainWidth * z + x;
			vertices[index].position = Vector3((float)x, 0.f, (float)z);
			vertices[index].normal = Vector3(0.f, 1.f, 0.f);
			vertices[index].Uv = Vector2((float)x / (float)terrainWidth, (float)(terrainHeight - 1 - z) / (float)terrainHeight);

		}
	}

	vector<UINT> indices;

	for (UINT z = 0; z < terrainHeight - 1; ++z)
	{
		for (UINT x = 0; x < terrainWidth - 1; ++x)
		{
			indices.push_back(terrainWidth * z + x);
			indices.push_back(terrainWidth * (z + 1) + x);
			indices.push_back(terrainWidth * z + x + 1);

			indices.push_back(terrainWidth * z + x + 1);
			indices.push_back(terrainWidth * (z + 1) + x);
			indices.push_back(terrainWidth * (z + 1) + x + 1);
		}
	}

	indexCount = indices.size();
	this->indices = new UINT[indexCount];

	copy(indices.begin(), indices.end(), this->indices);



	CreateStaticBuffer(Graphic->GetDevice(), vertices, vertexCount, sizeof(VertexType), D3D11_BIND_VERTEX_BUFFER, vertexBuffer.GetAddressOf());
	CreateStaticBuffer(Graphic->GetDevice(), this->indices, indexCount, sizeof(UINT), D3D11_BIND_INDEX_BUFFER, indexBuffer.GetAddressOf());

	return S_OK;
}

void TerrainBuffer::Render()
{
	RenderBuffers();
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

	for (UINT z = 0; z < terrainHeight - 1; ++z)
	{
		for (UINT x = 0; x < terrainWidth - 1; ++x)
		{
			int	index[4] = {-1};
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
			}
			if (ray.Intersects(vertex4, vertex2, vertex3, dist))
			{
				Pos = ray.position + ray.direction * dist;
			}
		}
	}

	brushDesc.position = Pos;
	brushBuffer->SetData(Graphic->GetDeviceContext(), brushDesc);
	auto buffer = brushBuffer->GetBuffer();
	Graphic->GetDeviceContext()->PSSetConstantBuffers(0, 1, &buffer);

	//cout << Pos.x << " , " << Pos.y << " , " << Pos.z << endl;
	return Pos;
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
	Graphic->GetDeviceContext()->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);

	Graphic->GetDeviceContext()->DrawIndexed(indexCount, 0, 0);
}

shared_ptr<TerrainBuffer> TerrainBuffer::Create(HWND _hWnd, UINT _terrainWidth, UINT _terrainHeight)
{
	shared_ptr<TerrainBuffer> Instance(new TerrainBuffer());
	if (FAILED(Instance->Initialize(_hWnd, _terrainWidth, _terrainHeight)))
	{
		MSG_BOX("Failed to create TerrainBuffer.");
		return nullptr;
	}
	return Instance;
}

int TerrainBuffer::Update(float _timeDelta)
{
	return 0;
}
