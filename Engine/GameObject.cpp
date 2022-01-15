#include "pch.h"
#include "GameObject.h"
#include "Component.h"

GameObject::GameObject()
{
	components.resize(COMPONENT_END);
}

GameObject::GameObject(const GameObject & Rhs)
{
}

GameObject::~GameObject()
{
}

void GameObject::RenderComponentsIspector()
{
	for (size_t i = 0; i < components.size(); ++i)
	{
		if (components[i])
			components[i]->RenderInspector();
	}
}


Vector3 GameObject::GetPosition()
{
	Vector3 pos = transform->GetState(Transform::POSITION);
	return pos;
}


shared_ptr<Component> GameObject::GetComponent(ComponentType componentType)
{
	if (components[componentType])
		return components[componentType];

	return nullptr;
}

void GameObject::AddComponent(ComponentType _componentType, shared_ptr<Component> _component)
{
	if (components[_componentType])
		return;

	components[_componentType] = _component;

}

void GameObject::UpdateComponent(float _timeDelta)
{
	for (UINT i = 0; i < components.size(); ++i)
	{
		if (components[i])
			components[i]->Update(_timeDelta);
	}
}

void GameObject::RenderComponent()
{
	for (UINT i = 0; i < components.size(); ++i)
	{
		if (components[i])
			components[i]->Render();
	}
}
