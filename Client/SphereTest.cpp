#include "pch.h"
#include "SphereTest.h"
#include "Transform.h"
#include "Shader.h"
#include "Sphere.h"
#include "Renderer.h"
#include "Material.h"
#include "Rigidbody.h"
#include "Collider.h"
#include "PhysXManager.h"

SphereTest::SphereTest()
{
}

SphereTest::SphereTest(const SphereTest & Rhs)
{
}

SphereTest::~SphereTest()
{
}

int SphereTest::Update(float _TimeDelta)
{
	UpdateComponent(_TimeDelta);

	Renderer::GetInstance()->AddRenderGroup(Renderer::RENDER_NONALPHA, shared_from_this());

	
	return 0;
}

void SphereTest::Render()
{
	RenderComponent();

	sphere->Render();

}

void SphereTest::OnContact()
{

}

void SphereTest::OnTrigger()
{
}

HRESULT SphereTest::Initialize()
{
	sphere = Sphere::Create(10.f);
	
	D3D11_INPUT_ELEMENT_DESC InputLayout[] =
	{
		{ "POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 0, D3D11_INPUT_PER_VERTEX_DATA, 0 },
		{ "NORMAL", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_VERTEX_DATA, 0 },
		{ "TEXCOORD", 0, DXGI_FORMAT_R32G32_FLOAT, 0, D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_VERTEX_DATA, 0 }

	};

	shader = Shader::Create(InputLayout, sizeof(InputLayout), L"../Engine/ColorVS.hlsl", L"../Engine/ColorPS.hlsl");
	components[ComponentType::SHADER] = shader;

	transform = Transform::Create(Transform::TRANSDESC());
	transform->SetObject(shared_from_this());
	transform->SetState(Transform::POSITION, Vector3(10.f, 15.f, 15.f));
	components[ComponentType::TRANSFORM] = transform;


	MATERIALDESC desc;
	material = Material::Create(desc);
	components[ComponentType::MATERIAL] = material;

	//rigidbody = Rigidbody::Create(shared_from_this(), RigidbodyType::DYNAMICRIGID);
	//components[ComponentType::RIGIDBODY] = rigidbody;
	//
	//collider = Collider::Create(shared_from_this(), PxGeometryType::eBOX);
	//components[ComponentType::COLLIDER] = collider;

	Vector3 pos = transform->GetState(Transform::POSITION);
	PhysXManager::GetInstance()->CreateController(controller, PxExtendedVec3((PxExtended)pos.x, (PxExtended)pos.y, (PxExtended)pos.z));
	return S_OK;
}

shared_ptr<SphereTest> SphereTest::Create()
{
	shared_ptr<SphereTest> Instance(new SphereTest());
	if (FAILED(Instance->Initialize()))
	{
		MSG_BOX("Failed to create SphereTest.");
		return nullptr;
	}
	return Instance;
}
