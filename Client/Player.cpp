#include "pch.h"
#include "Player.h"
#include "Transform.h"
#include "DynamicModel.h"
#include "Model.h"
#include "Management.h"

Player::Player()
{
}

Player::Player(const Player & Rhs)
{
}

Player::~Player()
{
}

int Player::Update(float _TimeDelta)
{
	TimeDelta += _TimeDelta;

	return 0;
}

void Player::Render()
{
	XMMATRIX world, view, proj;

	auto manage = Management::GetInstance();

	world = *transform->GetWorldMatrix();
	view = *manage->GetTransform(D3DTRANSFORMSTATE_VIEW);
	proj = *manage->GetTransform(D3DTRANSFORMSTATE_PROJECTION);

	world = XMMatrixTranspose(world);
	view = XMMatrixTranspose(view);
	proj = XMMatrixTranspose(proj);



	//Model->Render(MATRIXBUFFERTYPE(world, view, proj), TimeDelta);
}

HRESULT Player::Initialize(ID3D11Device * _Device)
{
	transform = Transform::Create(Transform::TRANSDESC(10.f, 10.f));
	//Model = DynamicModel::Create(_Device, "../Resources/Player/Jack_CB_IDL_01_Lp.FBX", L"../Engine/SkinnedMeshVS.hlsl", L"../Engine/SkinnedMeshPS.hlsl");
	model = new Model();

	return S_OK;
}

shared_ptr<Player> Player::Create(ID3D11Device * _Device)
{
	shared_ptr<Player> Instance(new Player);
	if (FAILED(Instance->Initialize(_Device)))
	{
		MSG_BOX("Failed to create Player.");
		return nullptr;
	}

	return Instance;
}
