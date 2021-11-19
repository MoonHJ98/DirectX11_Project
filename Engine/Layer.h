#pragma once
#include "Base.h"

class GameObject;

class Layer : public Base
{
public:
	virtual int Update(float _timeDelta) override;
	virtual void Render() override;

public:
	HRESULT		Initialize();
	void		AddGameObject(shared_ptr<GameObject> _GameObject);
	wstring		AddGameObject(wstring _key, shared_ptr<GameObject> _GameObject);

	list<shared_ptr<GameObject>>* FindGameObjectList() { return &GameObjects; }
	shared_ptr<GameObject> FindGameObject(int _ObjectIndex);

	map<wstring, pair<wstring, shared_ptr<GameObject>>>* FindGameObjectListTest() { return &objects; }
	shared_ptr<GameObject> FindGameObjectTest(wstring _objKey);

public:
	static shared_ptr<Layer> Create();

private:
	list<shared_ptr<GameObject>> GameObjects;
	map<wstring, pair<wstring, shared_ptr<GameObject>>> objects;
};

