#include "pch.h"
#include "Rigidbody.h"
#include "PhysXManager.h"
#include "Transform.h"
#include "GameObject.h"

Rigidbody::Rigidbody()
{
}

Rigidbody::Rigidbody(const Rigidbody & Rhs)
{
}

Rigidbody::~Rigidbody()
{
}

HRESULT Rigidbody::Initialize(shared_ptr<GameObject> _object, RigidbodyType _rigidbodyType)
{
	object = _object;

	componentType = RIGIDBODY;

	auto transComponent = object.lock()->GetComponents()[ComponentType::TRANSFORM];
	transform = dynamic_pointer_cast<Transform>(transComponent);

	switch (_rigidbodyType)
	{
	case DYNAMICRIGID:
		body = AddDynamicRigidbody(transform.lock()->GetState(Transform::POSITION));
		body->setActorFlag(PxActorFlag::eDISABLE_GRAVITY, true);
		break;
	case STATICRIGID:
		body = AddStaticRigidbody(transform.lock()->GetState(Transform::POSITION));
		break;
	case RIGIDBODY_END:
		break;
	default:
		break;
	}
	


	return S_OK;
}

PxRigidBody* Rigidbody::AddDynamicRigidbody(Vector3 _pos)
{
	return PhysXManager::GetInstance()->AddDynamicRigidbody(_pos);
}

PxRigidBody * Rigidbody::AddStaticRigidbody(Vector3 _pos)
{
	auto _body = PhysXManager::GetInstance()->AddDynamicRigidbody(_pos);
	_body->setRigidBodyFlag(PxRigidBodyFlag::eKINEMATIC, true);
	_body->setActorFlag(PxActorFlag::eDISABLE_GRAVITY, true);

	
	return _body;
}


int Rigidbody::Update(float _timeDelta)
{
	SetPosition();
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
	transform.lock()->SetState(Transform::POSITION, Vector3(body->getGlobalPose().p.x, body->getGlobalPose().p.y, body->getGlobalPose().p.z));
}


shared_ptr<Rigidbody> Rigidbody::Create(shared_ptr<GameObject> _object, RigidbodyType _rigidbodyType)
{
	shared_ptr<Rigidbody> Instance(new Rigidbody());
	if (FAILED(Instance->Initialize(_object, _rigidbodyType)))
	{
		MSG_BOX("Failed to create Rigidbody.");
		return nullptr;
	}
	return Instance;

}
