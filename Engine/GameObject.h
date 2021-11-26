#pragma once

#include "Base.h"

#include "Transform.h"

class GraphicDevice;
class Component;


class GameObject abstract : public Base
{
public:
	GameObject();
	GameObject(const GameObject& Rhs);
	~GameObject();

public:
	virtual int Update(float _TimeDelta) PURE;
	virtual void Render() PURE;
	void RenderComponentsIspector();
	void RenderDepthForShadow(BOOL _renderDepthForShadow) { renderDepthForShadow = _renderDepthForShadow; }
	bool GetisOrth() { return isOrtho; }
	BOOL GetRenderDeptehForShadow() { return renderDepthForShadow; }
public:
	Vector3 GetPosition() { return transform->GetState(Transform::POSITION); }
	vector<shared_ptr<Component>>& GetComponents() { return components; }

protected:
	shared_ptr<GraphicDevice> Graphic = nullptr;
	shared_ptr<Transform>	transform = nullptr;
	float					TimeDelta = 0;

	bool					isOrtho = false;
	BOOL                    renderDepthForShadow = FALSE;

	vector<shared_ptr<Component>> components;


};

