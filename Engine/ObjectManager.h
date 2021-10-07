#pragma once
#include "Base.h"

class Layer;
class GameObject;

class ObjectManager : public Base
{
	SINGLETON(ObjectManager)

	~ObjectManager();

public:
	typedef	map<wstring, Layer*>	LAYER;

public:
	virtual int Update(float _timeDelta) override;
	virtual void Render() override;

public:
	HRESULT					ReserveLayerMap(int _Size);
	void					AddLayer(int _Index, wstring _LayerKey, shared_ptr<GameObject> _Object);
	Layer*					FindLayer(int _Index, wstring _LayerKey);



private:
	shared_ptr<LAYER[]>		LayerMap = nullptr;
	UINT					LayerCnt = 0;

};

