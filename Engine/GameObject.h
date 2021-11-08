#pragma once

#include "Base.h"

class Transform;
class GraphicDevice;

class GameObject abstract : public Base
{
public:
	GameObject();
	GameObject(const GameObject& Rhs);
	~GameObject();

public:
	virtual int Update(float _TimeDelta) PURE;
	virtual void Render() PURE;
	void RenderDepthForShadow(BOOL _renderDepthForShadow) { renderDepthForShadow = _renderDepthForShadow; }


protected:
	shared_ptr<GraphicDevice> Graphic = nullptr;
	shared_ptr<Transform>	transform = nullptr;
	float					TimeDelta = 0;
	BOOL                    renderDepthForShadow = FALSE;
};

