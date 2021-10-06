#include "pch.h"
#include "ObjectManager.h"

int ObjectManager::Update(float _timeDelta)
{
	return 0;
}

void ObjectManager::Render()
{
}

HRESULT ObjectManager::ReserveLayerMap(int _Size)
{
	shared_ptr<LAYER[]> tempLayerMap(new LAYER[_Size]);
	LayerMap = tempLayerMap;

	LayerCnt = _Size;

	return S_OK;
}
