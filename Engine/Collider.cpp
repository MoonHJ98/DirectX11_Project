#include "pch.h"
#include "Collider.h"
#include "PhysXManager.h"
#include "GameObject.h"
#include "Rigidbody.h"
#include "ColliderRenderer.h"


Collider::Collider()
{
}

Collider::Collider(const Collider & Rhs)
{
}

Collider::~Collider()
{
}

HRESULT Collider::Initialize(shared_ptr<GameObject> _object, PxGeometryType::Enum _geoType)
{
	object = _object;
	componentType = COLLIDER;

	geoType = _geoType;
	shape = AddCollider();

	rigidbody = FindRigidbody();

	if (rigidbody.lock() != nullptr)
		AddColliderToRigidbody();
	else
		AddRigidBodyForCollider();

	colliderRenderer = ColliderRenderer::Create(object.lock(), geoType);

	return S_OK;
}

PxShape* Collider::AddCollider()
{
	ColliderDesc desc;
	desc.scale = Vector3(5.f, 5.f, 5.f);
	desc.radius = 5.f;
	desc.halfHeight = 10.f;

	return PhysXManager::GetInstance()->AddCollider(geoType, desc);
}

shared_ptr<Rigidbody> Collider::FindRigidbody()
{
	auto components = object.lock()->GetComponents();

	return dynamic_pointer_cast<Rigidbody>(components[ComponentType::RIGIDBODY]);
}

void Collider::AddColliderToRigidbody()
{
	rigidbody.lock()->GetRigidBody().attachShape(*shape);
}

void Collider::AddRigidBodyForCollider()
{
	auto rigidbody = Rigidbody::Create(object.lock(), RigidbodyType::STATICRIGID);
	rigidbody->GetRigidBody().attachShape(*shape);

	object.lock()->GetComponents()[ComponentType::RIGIDBODY] = rigidbody;

}

int Collider::Update(float _timeDelta)
{
	colliderRenderer->Update(_timeDelta);
	return 0;
}

void Collider::Render()
{
	colliderRenderer->Render();
}

void Collider::RenderInspector()
{
}

shared_ptr<Collider> Collider::Create(shared_ptr<GameObject> _object, PxGeometryType::Enum _geoType)
{
	shared_ptr<Collider> Instance(new Collider());
	if (FAILED(Instance->Initialize(_object, _geoType)))
	{
		MSG_BOX("Failed to create Collider.");
		return nullptr;
	}

	return Instance;
}
