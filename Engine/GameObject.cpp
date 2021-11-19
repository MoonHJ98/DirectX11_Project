#include "pch.h"
#include "GameObject.h"
#include "Component.h"

GameObject::GameObject()
{
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
