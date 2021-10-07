#pragma once
#include "Base.h"

class Scene;

class SceneManager : public Base
{
	SINGLETON(SceneManager)

	~SceneManager();

public:
	virtual int Update(float _timeDelta) override;
	virtual void Render() override;

public:
	HRESULT		SetScene(Scene* _scene);

private:
	Scene*		scene = nullptr;
};

