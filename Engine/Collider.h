#pragma once

#include "Component.h"

class Rigidbody;
class GameObject;
class ColliderRenderer;

class Collider : public Component
{
private:
	Collider();
	Collider(const Collider& Rhs);

public:
	~Collider();

private:
	HRESULT Initialize(shared_ptr<GameObject> _object, PxGeometryType::Enum _geoType);
	PxShape* AddCollider();
	shared_ptr<Rigidbody> FindRigidbody();
	void AddColliderToRigidbody();
	void AddRigidBodyForCollider();

private:
	void IsTrigger();
	void Transform();


public:
	// Component��(��) ���� ��ӵ�
	virtual int Update(float _timeDelta) override;
	virtual void Render() override;
	virtual void RenderInspector() override;

public:
	static shared_ptr<Collider> Create(shared_ptr<GameObject> _object, PxGeometryType::Enum _geoType);

private:
	PxShape* shape = nullptr;
	weak_ptr<Rigidbody> rigidbody;
	PxGeometryType::Enum geoType;
	
	shared_ptr<ColliderRenderer> colliderRenderer = nullptr;

	bool isTrigger = false;
	PxTransform transform;
};

