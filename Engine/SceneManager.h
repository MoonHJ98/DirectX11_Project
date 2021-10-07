#pragma once
#include "Base.h"

class Scene;

class SceneManager : public Base
{
	SINGLETON(SceneManager)

public:
	virtual int Update(float _timeDelta) override;
	virtual void Render() override;

public:
	HRESULT		SetScene(shared_ptr<Scene> _scene);

private:
	shared_ptr<Scene>		scene = nullptr;
};

