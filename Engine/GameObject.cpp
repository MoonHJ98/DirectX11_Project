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

shared_ptr<Component> GameObject::GetComponent(ComponentType componentType)
{
	if (components[componentType])
		return components[componentType];

	return nullptr;
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
