#include "pch.h"
#include "Player.h"
#include "Transform.h"
#include "Model.h"
#include "Management.h"
#include "Renderer.h"
#include "Light.h"
#include "GraphicDevice.h"
#include "Shader.h"





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

	for (size_t i = 0; i < components.size(); ++i)
	{
		components[i]->Update(_TimeDelta);
	}

	model->Update(_TimeDelta);


	Renderer::GetInstance()->AddRenderGroup(Renderer::RENDER_NONALPHA, shared_from_this());
	return 0;
}

void Player::Render()
{

	for (size_t i = 0; i < components.size(); ++i)
	{
		components[i]->Render();
	}

	model->Render();
}

HRESULT Player::Initialize(ID3D11Device * _Device)
{
	Graphic = GraphicDevice::GetInstance();


	transform = Transform::Create(Transform::TRANSDESC(10.f, 10.f));
	transform->SetObject(shared_from_this());
	transform->SetScale(Vector3(0.1f, 0.1f, 0.1f));
	transform->SetState(Transform::POSITION, Vector3(10.f, 0.f, 10.f));
	model = Model::Create();

	D3D11_INPUT_ELEMENT_DESC InputLayout[] =
	{
		{ "POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 0, D3D11_INPUT_PER_VERTEX_DATA, 0 },
		{ "TEXCOORD", 0, DXGI_FORMAT_R32G32_FLOAT, 0, D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_VERTEX_DATA, 0 },
		{ "NORMAL", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_VERTEX_DATA, 0 },
		{ "TANGENT", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_VERTEX_DATA, 0 },
		{ "BITANGENT", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_VERTEX_DATA, 0 },
		{ "BLENDWEIGHTS", 0, DXGI_FORMAT_R32G32B32A32_FLOAT, 0, D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_VERTEX_DATA, 0 },
		{ "BLENDINDICES", 0, DXGI_FORMAT_R32G32B32A32_FLOAT, 0, D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_VERTEX_DATA, 0 }
	};

	shader = Shader::Create(InputLayout, sizeof(InputLayout), L"../Engine/ModelVS.hlsl", L"../Engine/ModelPS.hlsl");

	components.push_back(transform);
	components.push_back(shader);

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
