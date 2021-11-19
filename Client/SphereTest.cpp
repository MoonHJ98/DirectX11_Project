#include "pch.h"
#include "SphereTest.h"
#include "Transform.h"
#include "Shader.h"
#include "Sphere.h"
#include "Renderer.h"

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
	Renderer::GetInstance()->AddRenderGroup(Renderer::RENDER_NONALPHA, shared_from_this());

	return 0;
}

void SphereTest::Render()
{
	shader->Render();
	transform->Update();

	sphere->Render();
}

HRESULT SphereTest::Initialize()
{
	sphere = Sphere::Create();
	
	D3D11_INPUT_ELEMENT_DESC InputLayout[] =
	{
		{ "POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 0, D3D11_INPUT_PER_VERTEX_DATA, 0 },
		{ "NORMAL", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_VERTEX_DATA, 0 },
		{ "TEXCOORD", 0, DXGI_FORMAT_R32G32_FLOAT, 0, D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_VERTEX_DATA, 0 }

	};

	shader = Shader::Create(InputLayout, sizeof(InputLayout), L"../Engine/ColorVS.hlsl", L"../Engine/ColorPS.hlsl");

	transform = Transform::Create(Transform::TRANSDESC());
	transform->SetObject(shared_from_this());

	transform->SetState(Transform::POSITION, Vector3(3.f, 0.f, 5.f));

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
