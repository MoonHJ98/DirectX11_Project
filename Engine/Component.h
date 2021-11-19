#pragma once
#include "Base.h"
class Component : public Base
{
public:
	Component();
	Component(const Component& Rhs);
	~Component();
	// Base��(��) ���� ��ӵ�
	virtual int Update(float _timeDelta) PURE;
	virtual void Render() PURE;
	virtual void RenderInspector() PURE;
};

