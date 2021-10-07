#include "pch.h"
#include "ObjectManager.h"
#include "Layer.h"

ObjectManager::~ObjectManager()
{
	for (int i = 0; i < LayerCnt; ++i)
	{
		for (auto iter = LayerMap[i].begin(); iter != LayerMap[i].end(); ++iter)
		{
			SAFEDELETE(iter->second);

		}
		LayerMap[i].clear();
	}
}

int ObjectManager::Update(float _timeDelta)
{
	int Res = 0;
	for (int i = 0; i < LayerCnt; ++i)
	{
		for (auto& Pair : LayerMap[i])
		{
			Res = Pair.second->Update(_timeDelta);
			//if (Res & 0x80000000)
			//	return Res;
		}
	}


	return Res;
}

void ObjectManager::Render()
{
	for (int i = 0; i < LayerCnt; ++i)
	{
		for (auto& Pair : LayerMap[i])
			Pair.second->Render();
	}
}

HRESULT ObjectManager::ReserveLayerMap(int _Size)
{
	shared_ptr<LAYER[]> tempLayerMap(new LAYER[_Size]);
	LayerMap = tempLayerMap;

	LayerCnt = _Size;

	return S_OK;
}

void ObjectManager::AddLayer(int _Index, wstring _LayerKey, shared_ptr<GameObject> _Object)
{
	auto layerFind = FindLayer(_Index, _LayerKey);
	if (layerFind == nullptr)
	{
		auto layer = Layer::Create();
		LayerMap[_Index].emplace(_LayerKey, layer);
	}

	LayerMap[_Index][_LayerKey]->AddGameObject(_Object);
}

Layer * ObjectManager::FindLayer(int _Index, wstring _LayerKey)
{
	auto iter = LayerMap[_Index].find(_LayerKey);

	if (iter == LayerMap[_Index].end())
		return nullptr;

	return iter->second;
}
