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
	HRESULT					      ReserveLayerMap(int _Size);

	void					      AddLayer(int _Index, wstring _LayerKey, shared_ptr<GameObject> _Object);
	void					      AddLayerTest(int _Index, wstring _LayerKey, shared_ptr<GameObject> _Object, bool _isRoot, shared_ptr<GameObject> _parent);
	void						  AddChildObject();
	shared_ptr<Layer>		      FindLayer(int _Index, wstring _LayerKey);


	list<shared_ptr<GameObject>>* FindGameObjectList(int _SceneIndex, wstring _LayerKey);
	shared_ptr<GameObject>        FindGameObject(int _SceneIndex, wstring _LayerKey, int _Objectindex);

	map<wstring, pair<wstring, shared_ptr<GameObject>>>* FindGameObjectListTest(int _SceneIndex, wstring _LayerKey);
	shared_ptr<GameObject>        FindGameObjectTest(int _SceneIndex, wstring _LayerKey, wstring _objKey);

	void						  AddObjectsforImgui(wstring _key, shared_ptr<GameObject> _object);
	map<wstring, shared_ptr<GameObject>>& GetObjectsForImgui() { return objectsForImgui; }

private:
	unique_ptr<LAYER[]>		LayerMap = nullptr;
	UINT					LayerCnt = 0;


	map<wstring, shared_ptr<GameObject>> objectsForImgui;

};

