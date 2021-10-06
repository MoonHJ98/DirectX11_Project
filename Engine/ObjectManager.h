#pragma once
#include "Base.h"

class Layer;

class ObjectManager : public Base
{
	SINGLETON(ObjectManager)

public:
	typedef	map<wstring, Layer*>	LAYER;

public:
	virtual int Update(float _timeDelta) override;
	virtual void Render() override;

public:
	HRESULT					ReserveLayerMap(int _Size);


private:
	shared_ptr<LAYER[]>		LayerMap = nullptr;
	UINT					LayerCnt = 0;

};

