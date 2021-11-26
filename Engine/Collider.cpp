#include "pch.h"
#include "Collider.h"
#include "PhysXManager.h"


Collider::Collider()
{
}

Collider::Collider(const Collider & Rhs)
{
}

Collider::~Collider()
{
}

HRESULT Collider::Initialize()
{
	componentName = L"Collider";


	return S_OK;
}

HRESULT Collider::AddCollider()
{
	return S_OK;
}

int Collider::Update(float _timeDelta)
{
	return 0;
}

void Collider::Render()
{
}

void Collider::RenderInspector()
{
}

shared_ptr<Collider> Collider::Create()
{
	shared_ptr<Collider> Instance(new Collider());
	if (FAILED(Instance->Initialize()))
	{
		MSG_BOX("Failed to create Collider.");
		return nullptr;
	}

	return Instance;
}
