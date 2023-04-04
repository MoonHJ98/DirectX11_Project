#include "pch.h"
#include "Terrain.h"
#include "Transform.h"
#include "Shader.h"
#include "Renderer.h"
#include "Management.h"
#include "Light.h"
#include "GraphicDevice.h"
#include "Material.h"
#include "TerrainBuffer.h"
#include "Management.h"
#include "StaticCamera.h"
#include "ImguiManager.h"
#include "Quadtree.h"
#include "PhysXManager.h"




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
		{ "TEXCOORD", 0, DXGI_FORMAT_R32G32_FLOAT, 0, D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_VERTEX_DATA, 0 },
		{ "COLOR", 0, DXGI_FORMAT_R32G32B32A32_FLOAT, 0, D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_VERTEX_DATA, 0 }

	};

	shader = Shader::Create(InputLayout, sizeof(InputLayout), L"../Engine/TerrainVS.hlsl", L"../Engine/TerrainPS.hlsl");

	MATERIALDESC desc;
	desc.name = L"Terrain";
	desc.DiffuseMap = L"../Resources/dirt01d.tga";

	material = Material::Create(desc);

	terrainBuffer = TerrainBuffer::Create(terrainWidth, _terrainHeight, GhWnd);
	//quadtree = Quadtree::Create(terrainBuffer);

	PhysXManager::GetInstance()->UpdateHeightField(terrainBuffer);

	components[ComponentType::TRANSFORM] = transform;
	components[ComponentType::SHADER] = shader;
	components[ComponentType::MATERIAL] = material;
	components[ComponentType::BUFFER] = terrainBuffer;

	physxManager = PhysXManager::GetInstance();

	return S_OK;
}

int Terrain::Update(float _TimeDelta)
{
	Renderer::GetInstance()->AddRenderGroup(Renderer::RENDER_NONALPHA, shared_from_this());

	for (size_t i = 0; i < components.size(); ++i)
	{
		if (components[i])
			components[i]->Update(_TimeDelta);
	}

	return 0;
}

void Terrain::Render()
{
	for (size_t i = 0; i < components.size(); ++i)
	{
		if (components[i])
			components[i]->Render();
	}
	//physxManager->UpdateHeightField(terrainBuffer);

	//quadtree->Render();

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

Vector3 Terrain::PickTerrain(Vector2 screenPos, Vector2 _screenSize)
{
	if (terrainBuffer == nullptr)
		return Vector3();

	if (screenPos.x < 0 || screenPos.x > _screenSize.x || screenPos.y < 0 || screenPos.y > _screenSize.y)
	{
		terrainBuffer->SetChangeTerrainToolRender(false);
		terrainBuffer->SetTerrainTool(FALSE);
		return Vector3();
	}

	//Vector3 Pos = quadtree->PickTerrain(screenPos, _screenSize);

	terrainBuffer->SetChangeTerrainToolRender(true);


	Vector3 Pos = terrainBuffer->PickTerrain(screenPos);

	//if (Manage->GetDIMouseState(InputDevice::DIM_LB) & 0x80)
	//{
	//	switch (terrainBuffer->GetTerrainToolStyle())
	//	{
	//	case TerrainBuffer::TerrainToolStyle::RaiseOrLowerTerrain:
	//		RaiseHeight();
	//		break;
	//	case TerrainBuffer::TerrainToolStyle::PaintTexture:
	//		PaintTexture();
	//		break;
	//	case TerrainBuffer::TerrainToolStyle::TerrainToolStyleEnd:
	//		break;
	//	default:
	//		break;
	//	}
	//}


	return Pos;
}

void Terrain::SetScreenSize(Vector2 _screenSize)
{
	terrainBuffer->SetScreenSize(_screenSize);
}

void Terrain::RaiseHeight()
{
	if (terrainBuffer == nullptr)
		return;


	terrainBuffer->RaiseHeight();
	physxManager->UpdateHeightField(terrainBuffer);

}

void Terrain::PaintTexture()
{
	terrainBuffer->DrawAlphaMap();
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


