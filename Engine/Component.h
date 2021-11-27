#pragma once

#include "Base.h"

class GameObject;
class Component : public Base
{
public:
	Component();
	Component(const Component& Rhs);
	~Component();
	// Base을(를) 통해 상속됨
	virtual int Update(float _timeDelta) PURE;
	virtual void Render() PURE;
	virtual void RenderInspector() PURE;
	
public:
	ComponentType componentType = COMPONENT_END;
	weak_ptr<GameObject>             object;
};

