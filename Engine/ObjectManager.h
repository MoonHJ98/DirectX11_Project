#pragma once
#include "Base.h"

class Layer;
class GameObject;

class ObjectManager : public Base
{
	SINGLETON(ObjectManager)

public:
	typedef	map<wstring, shared_ptr<Layer>>	LAYER;

public:
	virtual int Update(float _timeDelta) override;
	virtual void Render() override;

public:
	HRESULT					ReserveLayerMap(int _Size);
	void					AddLayer(int _Index, wstring _LayerKey, shared_ptr<GameObject> _Object);
	shared_ptr<Layer>		FindLayer(int _Index, wstring _LayerKey);



private:
	unique_ptr<LAYER[]>		LayerMap = nullptr;
	UINT					LayerCnt = 0;

};

