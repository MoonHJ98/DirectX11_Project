#pragma once

#include "Includes.h"

class Light;
class Shader;

class LightManager
{
	SINGLETON(LightManager)

private:
	typedef list<shared_ptr<Light>> LIGHTLIST;
	typedef map<wstring, LIGHTLIST> LIGHTMAP;

public:
	HRESULT				AddLight(LIGHTDESC _LightInfo, wstring _LightTag);
	HRESULT				FindLightList(wstring _LightTag);
	shared_ptr<Light>	FindLight(wstring _LightTag, UINT _Index);


private:
	LIGHTLIST			LightList;
	LIGHTMAP			LightMap;
};

