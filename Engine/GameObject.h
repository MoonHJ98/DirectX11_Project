#pragma once

#include "Base.h"

class Transform;

class GameObject abstract : public Base
{
public:
	GameObject();
	GameObject(const GameObject& Rhs);
	~GameObject();

public:
	virtual int Update(float _TimeDelta) PURE;
	virtual void Render() PURE;


protected:
	Transform*				transform = nullptr;
	float					TimeDelta = 0;
};

