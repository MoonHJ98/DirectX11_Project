#pragma once

#include "Base.h"

#include "Transform.h"
#include "Shader.h"

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
	Vector3 GetPosition();
	void SetPosition(Vector3 _position);
	vector<shared_ptr<Component>>& GetComponents() { return components; }
	shared_ptr<Component> GetComponent(ComponentType componentType);
	void AddComponent(ComponentType _componentType, shared_ptr<Component> _component);

public:
	virtual void OnContact() PURE;
	virtual void OnTrigger() PURE;

protected:
	void UpdateComponent(float _timeDelta);
	void RenderComponent();

protected:
	shared_ptr<GraphicDevice> Graphic = nullptr;
	shared_ptr<Transform>	transform = nullptr;
	shared_ptr<Shader>		shader = nullptr;
	float					TimeDelta = 0;

	bool					isOrtho = false;
	BOOL                    renderDepthForShadow = FALSE;

	vector<shared_ptr<Component>> components;


};

