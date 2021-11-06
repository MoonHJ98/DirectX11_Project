#include "pch.h"
#include "Player.h"
#include "Transform.h"
#include "Model.h"
#include "Management.h"
#include "Renderer.h"
#include "Light.h"

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
	model->Update(_TimeDelta);
	
	
	Renderer::GetInstance()->AddRenderGroup(Renderer::RENDER_NONALPHA, shared_from_this());
	return 0;
}

void Player::Render()
{
	transform->Update(false, renderDepthForShadow);

	model->Render();
	auto light = Management::GetInstance()->FindLight(L"DirectionalLight", 0);
	light->MatrixBufferToShader();
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
