#include "pch.h"
#include "Stage.h"
#include "StaticCamera.h"
#include "DynamicModel.h"

Stage::Stage()
{
}

Stage::Stage(const Stage & Rhs)
{
}

Stage::~Stage()
{
	delete camera;
}

HRESULT Stage::Initialize(ID3D11Device * _Device)
{
	Camera::CAMERADECS _decs;
	_decs.Far = SCREENDEPTH;
	_decs.Near = SCREENNEAR;
	_decs.FiedOfView = FoV;
	_decs.ScreenAspect = (float)GX / (float)GY;

	camera = StaticCamera::Create(_decs);

	model = DynamicModel::Create(_Device, "../Resources/Player/Jack_CB_IDL_01_Lp.FBX", L"../Engine/SkinnedMeshVS.hlsl", L"../Engine/SkinnedMeshPS.hlsl");
	return S_OK;
}

int Stage::Update(float _timeDelta)
{
	camera->Update(_timeDelta);
	model->Update(_timeDelta);
	return 0;
}

void Stage::Render()
{
	model->Render();
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
