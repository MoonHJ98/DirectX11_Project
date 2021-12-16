#include "pch.h"
#include "TerrainComponent.h"


TerrainComponent::TerrainComponent()
{
}

TerrainComponent::TerrainComponent(const TerrainComponent & Rhs)
{
}

TerrainComponent::~TerrainComponent()
{
}

int TerrainComponent::Update(float _timeDelta)
{
	return 0;
}

void TerrainComponent::Render()
{
}

void TerrainComponent::RenderInspector()
{
	if (ImGui::CollapsingHeader("Terrain"))
	{
		isTerrainComponentOpened = true;
	}
	else
		isTerrainComponentOpened = false;
}

HRESULT TerrainComponent::Initialize()
{


	return S_OK;
}

shared_ptr<TerrainComponent> TerrainComponent::Create()
{
	shared_ptr<TerrainComponent> Instance(new TerrainComponent());
	if (FAILED(Instance->Initialize()))
	{
		MSG_BOX("Failed to create TerrainComponent.");
		return nullptr;
	}
	return Instance;
}


