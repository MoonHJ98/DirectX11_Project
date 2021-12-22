#include "pch.h"
#include "Terrain.h"
#include "Transform.h"
#include "Shader.h"
#include "Renderer.h"
#include "Management.h"
#include "Light.h"
#include "GraphicDevice.h"
#include "Material.h"
#include "TerrainComponent.h"
#include "TerrainBuffer.h"
#include "Management.h"
#include "StaticCamera.h"
#include "ImguiManager.h"
#include "Quadtree.h"



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
	terrainWidth = _terrainWidth;
	terrainHeight = _terrainHeight;
	Graphic = GraphicDevice::GetInstance();
	Manage = Management::GetInstance();
	transform = Transform::Create(Transform::TRANSDESC());
	transform->SetObject(shared_from_this());

	D3D11_INPUT_ELEMENT_DESC InputLayout[] =
	{
		{ "POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 0, D3D11_INPUT_PER_VERTEX_DATA, 0 },
		{ "NORMAL", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_VERTEX_DATA, 0 },
		{ "TEXCOORD", 0, DXGI_FORMAT_R32G32_FLOAT, 0, D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_VERTEX_DATA, 0 }
	};

	shader = Shader::Create(InputLayout, sizeof(InputLayout), L"../Engine/ColorVS.hlsl", L"../Engine/ColorPS.hlsl");

	MATERIALDESC desc;
	desc.name = L"Terrain";
	desc.DiffuseMap = L"../Resources/dirt01d.tga";

	material = Material::Create(desc);

	terrainBuffer = TerrainBuffer::Create(/*"../Resources/heightmap.bmp"*/);
	quadtree = Quadtree::Create(terrainBuffer);


	terrainComponent = TerrainComponent::Create();

	components[ComponentType::TRANSFORM] = transform;
	components[ComponentType::SHADER] = shader;
	components[ComponentType::MATERIAL] = material;
	components[ComponentType::TERRAIN] = terrainComponent;


	return S_OK;
}

int Terrain::Update(float _TimeDelta)
{
	Renderer::GetInstance()->AddRenderGroup(Renderer::RENDER_NONALPHA, shared_from_this());

	for (size_t i = 0; i < components.size(); ++i)
	{
		if(components[i])
			components[i]->Update(_TimeDelta);
	}

	if(terrainComponent != nullptr && terrainComponent->IsTerrainComponentOpened())
		PickTerrain(screenPos);

	return 0;
}

void Terrain::Render()
{
	for (size_t i = 0; i < components.size(); ++i)
	{
		if (components[i])
			components[i]->Render();
	}

	quadtree->Render();

	//terrainBuffer->Render();
	//PickTerrain(Vector2());
	//heightTerrainBuffer->Render();
}

void Terrain::OnContact()
{
}

void Terrain::OnTrigger()
{
}

Vector3 Terrain::PickTerrain(Vector2 screenPos)
{
	Vector3 Pos = terrainBuffer->PickTerrain(screenPos);

	//cout << Pos.x << " , " << Pos.y << " , " << Pos.z << endl;
	return Pos;
}

void Terrain::SetScreenSize(Vector2 _screenSize)
{
	terrainBuffer->SetScreenSize(_screenSize);
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


