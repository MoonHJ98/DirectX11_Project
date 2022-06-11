#include "pch.h"
#include "ObjectManager.h"
#include "Layer.h"
#include "Management.h"


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

void ObjectManager::AddLayerTest(int _Index, wstring _LayerKey, shared_ptr<GameObject> _Object, bool _isRoot, shared_ptr<GameObject> _parent)
{
	auto layerFind = FindLayer(_Index, _LayerKey);
	if (layerFind == nullptr)
	{
		auto layer = Layer::Create();
		LayerMap[_Index].emplace(_LayerKey, layer);
	}

	wstring key = LayerMap[_Index][_LayerKey]->AddGameObject(_LayerKey, _Object);

	if(_isRoot)
		AddObjectsforImgui(key, _Object);

	if (_parent != nullptr)
		AddChildObject();
}

void ObjectManager::AddChildObject()
{
}

shared_ptr<Layer> ObjectManager::FindLayer(int _Index, wstring _LayerKey)
{
	auto iter = LayerMap[_Index].find(_LayerKey);

	if (iter == LayerMap[_Index].end())
		return nullptr;

	return iter->second;
}

list<shared_ptr<GameObject>>* ObjectManager::FindGameObjectList(int _SceneIndex, wstring _LayerKey)
{
	auto layer = FindLayer(_SceneIndex, _LayerKey);
	if (nullptr == layer)
	{
		MSG_BOX("Failed to find Layer.");
		return nullptr;
	}
	return layer->FindGameObjectList();
}

shared_ptr<GameObject> ObjectManager::FindGameObject(int _SceneIndex, wstring _LayerKey, int _Objectindex)
{
	auto layer = FindLayer(_SceneIndex, _LayerKey);
	if (nullptr == layer)
	{
		MSG_BOX("Failed to find layer.");
		return nullptr;
	}
	return layer->FindGameObject(_Objectindex);
}

map<wstring, pair<wstring, shared_ptr<GameObject>>>* ObjectManager::FindGameObjectListTest(int _SceneIndex, wstring _LayerKey)
{
	auto layer = FindLayer(_SceneIndex, _LayerKey);
	if (nullptr == layer)
	{
		MSG_BOX("Failed to find Layer.");
		return nullptr;
	}
	return layer->FindGameObjectListTest();
}

shared_ptr<GameObject> ObjectManager::FindGameObjectTest(int _SceneIndex, wstring _LayerKey, wstring _objKey)
{
	auto layer = FindLayer(_SceneIndex, _LayerKey);
	if (nullptr == layer)
	{
		MSG_BOX("Failed to find Layer.");
		return nullptr;
	}
	return layer->FindGameObjectTest(_objKey);
}

void ObjectManager::AddObjectsforImgui(wstring _key, shared_ptr<GameObject> _object)
{
	auto iter = objectsForImgui.find(_key);
	if (iter != objectsForImgui.end())
		return;

	objectsForImgui.emplace(make_pair(_key, _object));
}
