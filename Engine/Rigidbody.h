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
	HRESULT Initialize(shared_ptr<GameObject> _object, RigidbodyType _rigidbodyType);
	PxRigidBody* AddDynamicRigidbody(Vector3 _pos);
	PxRigidBody* AddStaticRigidbody(Vector3 _pos);

public:
	// Component��(��) ���� ��ӵ�
	virtual int Update(float _timeDelta) override;
	virtual void Render() override;
	virtual void RenderInspector() override;

public:
	PxRigidBody& GetRigidBody() { return *body; }
	
private:
	void SetPosition();

public:
	static shared_ptr<Rigidbody> Create(shared_ptr<GameObject> _object, RigidbodyType _rigidbodyType);

private:
	PxRigidBody* body = nullptr;
	weak_ptr<Transform> transform;

};

