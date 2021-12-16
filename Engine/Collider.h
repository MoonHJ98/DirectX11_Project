#pragma once

#include "Component.h"

class Rigidbody;
class GameObject;
class ColliderRenderer;

class Collider : public Component, public enable_shared_from_this<Collider>
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

public:
	PxTransform& GetLocalTransform() { return localTransform; }

private:
	void IsTrigger();
	void Transform();


public:
	// Component을(를) 통해 상속됨
	virtual int Update(float _timeDelta) override;
	virtual void Render() override;
	virtual void RenderInspector() override;

public:
	PxShape* GetShape() { return shape; }

public:
	static shared_ptr<Collider> Create(shared_ptr<GameObject> _object, PxGeometryType::Enum _geoType);

private:
	PxShape* shape = nullptr;
	weak_ptr<Rigidbody> rigidbody;
	PxGeometryType::Enum geoType;
	
	shared_ptr<ColliderRenderer> colliderRenderer = nullptr;

	bool isTrigger = false;
	PxTransform localTransform;

};

