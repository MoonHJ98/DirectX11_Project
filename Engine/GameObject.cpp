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
		components[i]->RenderInspector();
	}
}
