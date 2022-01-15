#include "pch.h"
#include "Management.h"
#include "GraphicDevice.h"
#include "SceneManager.h"
#include "ObjectManager.h"
#include "PipeLine.h"
#include "GameObject.h"
#include "Scene.h"
#include "LightManager.h"
#include "Renderer.h"

int Management::Update(float _timeDelta)
{
	SceneMgr->Update(_timeDelta);

	ObjectMgr->Update(_timeDelta);

	return 0;
}

void Management::Render()
{
	SceneMgr->Render();

	renderer->Render();
}

void Management::UpdateInputDevice()
{
	InputDevice::GetInstance()->UpdateInputDev();
}

UCHAR Management::GetDIKeyState(UCHAR KeyID)
{
	return InputDevice::GetInstance()->GetDIKeyState(KeyID);
}

UCHAR Management::GetDIMouseState(InputDevice::MOUSEKEYSTATE _MouseState)
{
	return InputDevice::GetInstance()->GetDIMouseState(_MouseState);
}

LONG Management::GetDIMouseMove(InputDevice::MOUSEMOVESTATE _MouseState)
{
	return InputDevice::GetInstance()->GetDIMouseMove(_MouseState);
}

Matrix * Management::GetTransform(_D3DTRANSFORMSTATETYPE _Type)
{
	return Pipeline->GetTransform(_Type);

}

void Management::AddLayer(int _Index, wstring _LayerKey, shared_ptr<GameObject> _Object)
{
	ObjectMgr->AddLayer(_Index, _LayerKey, _Object);
}

void Management::AddLayerTest(int _Index, wstring _LayerKey, shared_ptr<GameObject> _Object, bool _isRoot)
{
	ObjectMgr->AddLayerTest(_Index, _LayerKey, _Object, _isRoot);

}

HRESULT Management::SetScene(shared_ptr<Scene> scene)
{
	if (SceneMgr == nullptr)
		return E_FAIL;

	return SceneMgr->SetScene(scene);
}

HRESULT Management::AddLight(LIGHTDESC _LightInfo, wstring _LightTag)
{
	return LightMgr->AddLight(_LightInfo, _LightTag);
}

shared_ptr<Light> Management::FindLight(wstring _LightTag, UINT _Index)
{
	return LightMgr->FindLight(_LightTag, _Index);
}

list<shared_ptr<GameObject>>* Management::FindGameObjectList(int _Index, wstring _LayerKey)
{
	return ObjectMgr->FindGameObjectList(_Index, _LayerKey);
}

shared_ptr<GameObject> Management::FindGameObject(int _SceneIndex, wstring _LayerKey, int _ObjectIndex)
{
	return ObjectMgr->FindGameObject(_SceneIndex, _LayerKey, _ObjectIndex);
}

map<wstring, pair<wstring, shared_ptr<GameObject>>>* Management::FindGameObjectListTest(int _Index, wstring _LayerKey)
{
	return ObjectMgr->FindGameObjectListTest(_Index, _LayerKey);
}

shared_ptr<GameObject> Management::FindGameObjectTest(int _SceneIndex, wstring _LayerKey, wstring _objKey)
{
	return ObjectMgr->FindGameObjectTest(_SceneIndex, _LayerKey, _objKey);
}

shared_ptr<Frustum>& Management::GetFrustum()
{
	return renderer->GetFrustum();
}

map<wstring, shared_ptr<GameObject>>& Management::GetObjectsForImgui()
{
	return ObjectMgr->GetObjectsForImgui();
}


void Management::Initialize(HINSTANCE hInst, HWND hWnd, int SceneCnt, int screenWidth, int screenHeight, bool vsync, bool fullscreen, float screenDepth, float screenNear)
{
	SceneMgr = SceneManager::GetInstance();
	ObjectMgr = ObjectManager::GetInstance();
	Pipeline = PipeLine::GetInstance();
	LightMgr = LightManager::GetInstance();
	renderer = Renderer::GetInstance();

	GraphicDevice::GetInstance()->Initialize(screenWidth, screenHeight, vsync, hWnd, fullscreen, screenDepth, screenNear);
	InputDevice::GetInstance()->Initialize(hInst, hWnd, screenWidth, screenHeight);

	ObjectMgr->ReserveLayerMap(SceneCnt);
	renderer->Initialize();
}
