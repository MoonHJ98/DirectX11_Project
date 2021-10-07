#include "pch.h"
#include "SceneManager.h"
#include "Scene.h"


SceneManager::~SceneManager()
{
	SAFEDELETE(scene);
}

int SceneManager::Update(float _timeDelta)
{
	if (!scene)
		return -1;

	return scene->Update(_timeDelta);
}

void SceneManager::Render()
{
	scene->Render();
}

HRESULT SceneManager::SetScene(Scene * _scene)
{
	SAFEDELETE(scene);

	scene = _scene;

	return S_OK;
}
