#include "pch.h"
#include "Stage.h"

Stage::Stage()
{
}

Stage::Stage(const Stage & Rhs)
{
}

Stage::~Stage()
{
}

HRESULT Stage::Initialize(ID3D11Device * _Device)
{
	return S_OK;
}

int Stage::Update(float _timeDelta)
{
	return 0;
}

void Stage::Render()
{
}

Stage * Stage::Create(ID3D11Device * _Device)
{
	auto Instance = new Stage();
	if (FAILED(Instance->Initialize(_Device)))
	{
		MSG_BOX("Failed to create Stage.");
		return nullptr;
	}
	return Instance;
}
