#pragma once

#include "Component.h"

class Transform;

class Rigidbody : public Component
{
private:
	Rigidbody();
	Rigidbody(const Rigidbody& Rhs);

public:
	~Rigidbody();

private:
	HRESULT Initialize(shared_ptr<Transform> _transform);
	PxRigidBody* AddRigidbody(Vector3 _pos);

public:
	// Component을(를) 통해 상속됨
	virtual int Update(float _timeDelta) override;
	virtual void Render() override;
	virtual void RenderInspector() override;

private:
	void SetPosition();

public:
	static shared_ptr<Rigidbody> Create(shared_ptr<Transform> _transform);

private:
	PxRigidBody* body = nullptr;
	shared_ptr<Transform> transform = nullptr;

};

