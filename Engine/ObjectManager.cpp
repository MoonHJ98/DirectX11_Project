#include "pch.h"
#include "ObjectManager.h"
#include "Layer.h"


int ObjectManager::Update(float _timeDelta)
{
	int Res = 0;
	for (UINT i = 0; i < LayerCnt; ++i)
	{
		for (auto& Pair : LayerMap[i])
		{
			Res = Pair.second->Update(_timeDelta);
			if (Res & 0x80000000)
				return Res;
		}
	}


	return Res;
}

void ObjectManager::Render()
{
	for (UINT i = 0; i < LayerCnt; ++i)
	{
		for (auto& Pair : LayerMap[i])
			Pair.second->Render();
	}
}

HRESULT ObjectManager::ReserveLayerMap(int _Size)
{
	unique_ptr<LAYER[]> tempLayerMap(new LAYER[_Size]);
	LayerMap = move(tempLayerMap);

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

shared_ptr<Layer> ObjectManager::FindLayer(int _Index, wstring _LayerKey)
{
	auto iter = LayerMap[_Index].find(_LayerKey);

	if (iter == LayerMap[_Index].end())
		return nullptr;

	return iter->second;
}
