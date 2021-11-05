#include "pch.h"
#include "Player.h"
#include "Transform.h"
#include "Model.h"
#include "Management.h"
#include "Renderer.h"

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
	transform->Update();
	model->Update(_TimeDelta);
	
	
	Renderer::GetInstance()->AddRenderGroup(Renderer::RENDER_NONALPHA, shared_from_this());
	return 0;
}

void Player::Render()
{
	model->Render();
}

HRESULT Player::Initialize(ID3D11Device * _Device)
{
	transform = Transform::Create(Transform::TRANSDESC(10.f, 10.f));
	transform->SetScale(Vector3(0.1f, 0.1f, 0.1f));
	model = Model::Create();
	
	
	return S_OK;
}

shared_ptr<Player> Player::Create(ID3D11Device * _Device)
{
	shared_ptr<Player> Instance(new Player());
	if (FAILED(Instance->Initialize(_Device)))
	{
		MSG_BOX("Failed to create Player.");
		return nullptr;
	}

	return Instance;
}
