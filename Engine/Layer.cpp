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
			(*iter).reset();
			
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

void Layer::AddGameObject(shared_ptr<GameObject> _GameObject)
{
	GameObjects.push_back(_GameObject);
}

wstring Layer::AddGameObject(wstring _key, shared_ptr<GameObject> _GameObject)
{
	UINT count = 0;
	wstring pairFirst = _key;
	
	for (auto& iter : objects)
	{
		if (iter.second.first == pairFirst)
			++count;

		wstring key = _key + L" (" + to_wstring(count - 1) + L")";

		auto findkey = objects.find(key);

		if (findkey == objects.end())
		{
			objects.emplace(key, make_pair(pairFirst, _GameObject));
			return key;
		}

	}

	objects.emplace(_key, make_pair(pairFirst, _GameObject));
	return _key;
}


shared_ptr<GameObject> Layer::FindGameObject(int _ObjectIndex)
{
	auto iter = GameObjects.begin();

	for (int i = 0; i < _ObjectIndex; ++i)
	{
		++iter;
	}

	return *iter;
}

shared_ptr<Layer> Layer::Create()
{
	shared_ptr<Layer> Instance(new Layer());
	if (FAILED(Instance->Initialize()))
	{
		MSG_BOX("Failed to create Layer.");
		return nullptr;
	}

	return Instance;
}
