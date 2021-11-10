#include "pch.h"
#include "Terrain.h"
#include "HeightTerrainBuffer.h"
#include "Transform.h"
#include "Shader.h"
#include "Renderer.h"
#include "Management.h"
#include "Light.h"
#include "GraphicDevice.h"
#include "Material.h"
#include "TerrainBuffer.h"
#include "Management.h"


Terrain::Terrain()
{
}

Terrain::Terrain(const Terrain & Rhs)
{
}

Terrain::~Terrain()
{
}

HRESULT Terrain::Initialize(UINT _terrainWidth, UINT _terrainHeight, wstring _heightMapPath)
{
	Graphic = GraphicDevice::GetInstance();
	Manage = Management::GetInstance();
	heightTerrainBuffer = HeightTerrainBuffer::Create(_terrainWidth, _terrainHeight, _heightMapPath);
	transform = Transform::Create(Transform::TRANSDESC());

	D3D11_INPUT_ELEMENT_DESC InputLayout[] =
	{
		{ "POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 0, D3D11_INPUT_PER_VERTEX_DATA, 0 },
		{ "TEXCOORD", 0, DXGI_FORMAT_R32G32_FLOAT, 0, D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_VERTEX_DATA, 0 },
		{ "NORMAL", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_VERTEX_DATA, 0 }

	};

	shader = Shader::Create(InputLayout, sizeof(InputLayout), L"../Engine/ColorVS.hlsl", L"../Engine/ColorPS.hlsl");

	MATERIALDESC desc;
	desc.name = L"Terrain";
	desc.DiffuseMap = L"../Resources/dirt01d.tga";

	material = Material::Create(desc);

	terrainBuffer = TerrainBuffer::Create(100, 100);

	return S_OK;
}

int Terrain::Update(float _TimeDelta)
{
	Renderer::GetInstance()->AddRenderGroup(Renderer::RENDER_NONALPHA, shared_from_this());

	POINT p;
	GetCursorPos(&p);
	ScreenToClient(GhWnd, &p);


	Matrix	matProj;
	matProj = *Manage->GetTransform(D3DTRANSFORMSTATE_PROJECTION);


	// 뷰포트의 마우스를 투영의 마우스로 변환 -> 뷰스페이스로 변환
	Vector3		Temp;

	Temp.x = (float(p.x) / (GX >> 1) - 1.f) / matProj._11;
	Temp.y = (float(-p.y) / (GY >> 1) + 1.f) / matProj._22;
	Temp.z = 1.f;

	// 뷰 스페이스 상의 rayPos, rayDir
	Vector3 RayPos = Vector3(0.f, 0.f, 0.f);
	Vector3 RayDir = Temp - RayPos;
	RayDir.Normalize();

	// 월드로 변환
	Matrix	matView;
	matView = *Manage->GetTransform(D3DTRANSFORMSTATE_VIEW);
	matView.Invert();
	XMVector3TransformCoord(RayPos, matView);
	XMVector3TransformNormal(RayDir, matView);


	Ray ray;


	for (int z = 0; z < 100 - 1; ++z)
	{
		for (int x = 0; x < 100 - 1; ++x)
		{
			int	index = z * 100 + x;

			auto vertices = terrainBuffer->GetVertices();


			Vector3 vertex1 = vertices[index + 100 + 1].position;
			Vector3 vertex2 = vertices[index + 1].position;
			Vector3 vertex3 = vertices[index + 100].position;
			
			float dist = 0.f;
			ray.position = RayPos;

			ray.direction = RayDir;
			if (ray.Intersects(vertex1, vertex2, vertex3, dist))
			{
				int a = 10;
			}

		}
	}
	

	return 0;
}

void Terrain::Render()
{
	shader->Render();


	material->Render();
	transform->Update(false, renderDepthForShadow);
	terrainBuffer->Render();
	//heightTerrainBuffer->Render();
}

shared_ptr<Terrain> Terrain::Create(UINT _terrainWidth, UINT _terrainHeight, wstring _heightMapPath)
{
	shared_ptr<Terrain> Instance(new Terrain());
	if (FAILED(Instance->Initialize(_terrainWidth, _terrainHeight, _heightMapPath)))
	{
		MSG_BOX("Failed to create Terrain.");
		return nullptr;
	}
	return Instance;
}


