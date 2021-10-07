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
	void		AddGameObject(GameObject* _GameObject);


public:
	static Layer* Create();

private:
	list<GameObject*> GameObjects;
};

