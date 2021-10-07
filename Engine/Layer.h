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


public:
	static shared_ptr<Layer> Create();

private:
	list<shared_ptr<GameObject>> GameObjects;
};

