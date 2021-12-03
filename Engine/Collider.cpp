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

	transform = shape->getLocalPose();

	return S_OK;
}

PxShape* Collider::AddCollider()
{
	ColliderDesc desc;
	desc.scale = Vector3(5.f, 5.f, 5.f);
	desc.radius = 10.f;
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
	auto rigid = Rigidbody::Create(object.lock());
	rigidbody = rigid;

	rigidbody.lock()->GetRigidBody().attachShape(*shape);

	object.lock()->GetComponents()[ComponentType::RIGIDBODY] = rigidbody.lock();

}

void Collider::IsTrigger()
{
	if (ImGui::Checkbox("Is Trigger", &isTrigger))
	{
		if (isTrigger)
			rigidbody.lock()->GetRigidBody().detachShape(*shape);
		else
			rigidbody.lock()->GetRigidBody().attachShape(*shape);

	}

}

void Collider::Transform()
{
	float pos[3] = { transform.p.x, transform.p.y, transform.p.z };
	
	ImGui::InputFloat3("Center", pos);
	transform.p = PxVec3(pos[0], pos[1], pos[2]);

	PxTransform localTm(pos[0], pos[1], pos[2]);
	PxTransform transform(PxVec3(0));

	shape->setLocalPose(transform.transform(localTm));
	
	float size[3] = {};
	ImGui::InputFloat3("Size", size);

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
	if (ImGui::CollapsingHeader("Collider"))
	{
		IsTrigger();
		Transform();

	}
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
