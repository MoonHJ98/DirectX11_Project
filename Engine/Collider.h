#pragma once
#include "Component.h"
class Collider : public Component
{
private:
	Collider();
	Collider(const Collider& Rhs);

public:
	~Collider();

private:
	HRESULT Initialize();
	HRESULT AddCollider();


public:
	// Component을(를) 통해 상속됨
	virtual int Update(float _timeDelta) override;
	virtual void Render() override;
	virtual void RenderInspector() override;

public:
	static shared_ptr<Collider> Create();

private:
	PxShape* shape = nullptr;
};

