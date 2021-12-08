#include "pch.h"
#include "Rigidbody.h"
#include "PhysXManager.h"
#include "Transform.h"
#include "GameObject.h"
#include "Collider.h"

Rigidbody::Rigidbody()
{
}

Rigidbody::Rigidbody(const Rigidbody & Rhs)
{
}

Rigidbody::~Rigidbody()
{
}

HRESULT Rigidbody::Initialize(shared_ptr<GameObject> _object)
{
	object = _object;

	componentType = RIGIDBODY;

	auto transComponent = object.lock()->GetComponents()[ComponentType::TRANSFORM];
	transform = dynamic_pointer_cast<Transform>(transComponent);

	Vector3 pos = transform.lock()->GetState(Transform::POSITION);
	body = AddRigidbody(pos);
	body->setActorFlag(PxActorFlag::eDISABLE_GRAVITY, true);




	return S_OK;
}

PxRigidDynamic* Rigidbody::AddRigidbody(Vector3 _pos)
{
	return PhysXManager::GetInstance()->AddRigidbody(_pos);
}

void Rigidbody::Mass()
{
	float mass = body->getMass();
	ImGui::InputFloat("Mass", &mass);
	body->setMass(PxReal(mass));

}

void Rigidbody::Gravity()
{
	if (ImGui::Checkbox("Use Gravity", &useGravity))
	{
		if (useGravity)
		{
			body->setActorFlag(PxActorFlag::eDISABLE_GRAVITY, false);
			body->wakeUp();
		}
		else
		{
			body->setActorFlag(PxActorFlag::eDISABLE_GRAVITY, true);
			body->putToSleep();
		}

	}

}

void Rigidbody::Kinematic()
{
	if (ImGui::Checkbox("is Kinematic", &isKinematic))
	{
		if (isKinematic)
			body->setRigidBodyFlag(PxRigidBodyFlag::eKINEMATIC, isKinematic);
		else
			body->setRigidBodyFlag(PxRigidBodyFlag::eKINEMATIC, isKinematic);
	}
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
	if (ImGui::CollapsingHeader("Rigidbody"))
	{
		Mass();
		Gravity();
		Kinematic();
	}
}

void Rigidbody::SetPosition()
{
	//auto collider = dynamic_pointer_cast<Collider>(object.lock()->GetComponent(ComponentType::COLLIDER));
	//PxVec3 localPos = collider->GetShape()->getLocalPose().p;
	PxVec3 bodyPos = body->getGlobalPose().p;

	PxVec3 pos = bodyPos;

	transform.lock()->SetState(Transform::POSITION, Vector3(pos.x, pos.y, pos.z));
}


shared_ptr<Rigidbody> Rigidbody::Create(shared_ptr<GameObject> _object)
{
	shared_ptr<Rigidbody> Instance(new Rigidbody());
	if (FAILED(Instance->Initialize(_object)))
	{
		MSG_BOX("Failed to create Rigidbody.");
		return nullptr;
	}
	return Instance;

}
