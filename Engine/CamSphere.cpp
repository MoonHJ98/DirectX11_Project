#include "pch.h"
#include "CamSphere.h"
#include "GraphicDevice.h"
#include "Sphere.h"

CamSphere::CamSphere()
{
}

CamSphere::CamSphere(const CamSphere & Rhs)
{
}

CamSphere::~CamSphere()
{
}

int CamSphere::Update(float _TimeDelta)
{
	UpdateComponent(_TimeDelta);

	return 0;
}

void CamSphere::Render()
{
	RenderComponent();

}

void CamSphere::OnContact()
{
}

void CamSphere::OnTrigger()
{
}

HRESULT CamSphere::Initialize(Vector3 _pos, Vector3 _color)
{
	Graphic = GraphicDevice::GetInstance();

	transform = Transform::Create(Transform::TRANSDESC());

	transform->SetObject(shared_from_this());

	transform->SetState(Transform::POSITION, _pos);


	D3D11_INPUT_ELEMENT_DESC InputLayout[] =
	{
		{ "POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 0, D3D11_INPUT_PER_VERTEX_DATA, 0 },
		{ "COLOR", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_VERTEX_DATA, 0 },
		{ "TEXCOORD", 0, DXGI_FORMAT_R32G32_FLOAT, 0, D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_VERTEX_DATA, 0 }

	};

	shader = Shader::Create(InputLayout, sizeof(InputLayout), L"../Engine/ColliderVS.hlsl", L"../Engine/ColliderPS.hlsl");
	sphere = Sphere::Create(2.5f, _color);


	components[ComponentType::SHADER] = shader;
	components[ComponentType::TRANSFORM] = transform;
	components[ComponentType::BUFFER] = sphere;

	return S_OK;
}

shared_ptr<CamSphere> CamSphere::Create(Vector3 _pos, Vector3 _color)
{
	shared_ptr<CamSphere> Instance(new CamSphere());
	if(FAILED(Instance->Initialize(_pos, _color)))
	{
		MSG_BOX("Failed to create CamSphere.");
		return nullptr;
	}
	return Instance;
}
