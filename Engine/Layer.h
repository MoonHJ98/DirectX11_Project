#pragma once
#include "Base.h"

class GameObject;

class Layer : public Base
{
	virtual int Update(float _timeDelta) override;
	virtual void Render() override;

public:
	HRESULT		Initialize();


public:
	static Layer* Create();

private:
	list<GameObject*> GameObjects;
};

