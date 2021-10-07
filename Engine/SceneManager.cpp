#include "pch.h"
#include "SceneManager.h"
#include "Scene.h"


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

HRESULT SceneManager::SetScene(shared_ptr<Scene> _scene)
{
	scene.reset();

	scene = _scene;

	return S_OK;
}
