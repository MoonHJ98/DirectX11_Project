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
	int a = 10;
	return 0;
}

void Stage::Render()
{
	int a = 10;

}

shared_ptr<Stage> Stage::Create(ID3D11Device * _Device)
{
	shared_ptr<Stage> Instance(new Stage());
	if (FAILED(Instance->Initialize(_Device)))
	{
		MSG_BOX("Failed to create Stage.");
		return nullptr;
	}
	return Instance;
}
