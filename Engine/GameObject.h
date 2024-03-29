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
	bool GetisBillboard() { return isBillboard; }
	BOOL GetRenderDeptehForShadow() { return renderDepthForShadow; }
	void ComputeViewZ(Vector3* _worldPos);

public:
	Vector3 GetPosition();
	void SetPosition(Vector3 _position);
	vector<shared_ptr<Component>>& GetComponents() { return components; }
	shared_ptr<Component> GetComponent(ComponentType componentType);
	void AddComponent(ComponentType _componentType, shared_ptr<Component> _component);

	void SetIsRoot(bool _root) { isRoot = _root; }
	bool GetIsRoot() { return isRoot; }
	float GetViewZ() const { return viewZ; }

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

	bool					isBillboard = false;
	BOOL                    renderDepthForShadow = FALSE;

	vector<shared_ptr<Component>> components;

	bool isRoot = true;

	vector<shared_ptr<GameObject>> childObject;

	float viewZ = 0.f;
};

