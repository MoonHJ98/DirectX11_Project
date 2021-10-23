#include "pch.h"
#include "LightManager.h"
#include "Light.h"

HRESULT LightManager::AddLight(LIGHTDESC _LightInfo, wstring _LightTag)
{
	shared_ptr<Light> light = Light::Create(_LightInfo);
	if (nullptr == light)
		return E_FAIL;

	if (FAILED(FindLightList(_LightTag)))
	{
		LightMap.emplace(_LightTag, LIGHTLIST());
	}

	LightMap[_LightTag].emplace_back(light);
	return S_OK;
}

HRESULT LightManager::FindLightList(wstring _LightTag)
{
	auto iter = LightMap.find(_LightTag);

	if (iter == LightMap.end())
		return E_FAIL;

	return S_OK;
}

shared_ptr<Light> LightManager::FindLight(wstring _LightTag, UINT _Index)
{
	if (FAILED(FindLightList(_LightTag)))
		return nullptr;

	if (_Index >= LightMap[_LightTag].size())
		return nullptr;

	auto light = LightMap[_LightTag].begin();
	
	for (UINT i = 0; i < _Index; ++i)
	{
		++light;
	}

	return *light;
}
