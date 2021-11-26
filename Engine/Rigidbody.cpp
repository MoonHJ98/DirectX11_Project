#include "pch.h"
#include "Rigidbody.h"
#include "PhysXManager.h"
#include "Transform.h"

Rigidbody::Rigidbody()
{
}

Rigidbody::Rigidbody(const Rigidbody & Rhs)
{
}

Rigidbody::~Rigidbody()
{
}

HRESULT Rigidbody::Initialize(shared_ptr<Transform> _transform)
{
	componentName = L"Rigidbody";
	transform = _transform;
	body = AddRigidbody(transform->GetState(Transform::POSITION));

	return S_OK;
}

PxRigidBody* Rigidbody::AddRigidbody(Vector3 _pos)
{
	return PhysXManager::GetInstance()->AddRigidbody(_pos);
}

int Rigidbody::Update(float _timeDelta)
{
	//SetPosition();
	return 0;
}

void Rigidbody::Render()
{
}

void Rigidbody::RenderInspector()
{
}

void Rigidbody::SetPosition()
{
	transform->SetState(Transform::POSITION, Vector3(body->getGlobalPose().p.x, body->getGlobalPose().p.y, body->getGlobalPose().p.z));
}


shared_ptr<Rigidbody> Rigidbody::Create(shared_ptr<Transform> _transform)
{
	shared_ptr<Rigidbody> Instance(new Rigidbody());
	if (FAILED(Instance->Initialize(_transform)))
	{
		MSG_BOX("Failed to create Rigidbody.");
		return nullptr;
	}
	return Instance;

}
