#include "pch.h"
#include "Layer.h"
#include "GameObject.h"

int Layer::Update(float _timeDelta)
{
	int Res = 0;
	for (auto iter = GameObjects.begin(); iter != GameObjects.end();)
	{
		Res = (*iter)->Update(_timeDelta);
		if (Res & 0x80000000)
		{
			delete *iter;
			iter = GameObjects.erase(iter);
		}
		else
		{
			++iter;
		}
	}
	return Res;
}

void Layer::Render()
{
	for (auto iter = GameObjects.begin(); iter != GameObjects.end(); ++iter)
	{
		(*iter)->Render();
	}
}

HRESULT Layer::Initialize()
{
	return S_OK;
}

void Layer::AddGameObject(GameObject * _GameObject)
{
	GameObjects.push_back(_GameObject);
}

Layer * Layer::Create()
{
	auto Instance = new Layer();
	if (FAILED(Instance->Initialize()))
	{
		MSG_BOX("Failed to create Layer.");
		return nullptr;
	}

	return Instance;
}
