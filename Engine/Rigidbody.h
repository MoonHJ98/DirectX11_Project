#pragma once

#include "Component.h"

class Transform;
class GameObject;


class Rigidbody : public Component
{
private:
	Rigidbody();
	Rigidbody(const Rigidbody& Rhs);

public:
	~Rigidbody();

private:
	HRESULT Initialize(shared_ptr<GameObject> _object);
	PxRigidDynamic* AddRigidbody(Vector3 _pos);

	void Mass();
	void Gravity();
	void Kinematic();
	

public:
	// Component을(를) 통해 상속됨
	virtual int Update(float _timeDelta) override;
	virtual void Render() override;
	virtual void RenderInspector() override;

public:
	PxRigidDynamic* GetRigidBody() { return body; }
	
private:
	void SetPosition();

public:
	static shared_ptr<Rigidbody> Create(shared_ptr<GameObject> _object);

private:
	PxRigidDynamic* body = nullptr;
	PxRigidActor* actor = nullptr;
	weak_ptr<Transform> transform;

	bool useGravity = false;
	bool isKinematic = false;

};

