#include "pch.h"
#include "Management.h"
#include "GraphicDevice.h"
#include "SceneManager.h"
#include "ObjectManager.h"
#include "PipeLine.h"
#include "GameObject.h"
#include "Scene.h"

int Management::Update(float _timeDelta)
{
	SceneMgr->Update(_timeDelta);

	ObjectMgr->Update(_timeDelta);

	return 0;
}

void Management::Render()
{
	SceneMgr->Render();

	ObjectMgr->Render();
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

XMMATRIX * Management::GetTransform(_D3DTRANSFORMSTATETYPE _Type)
{
	return Pipeline->GetTransform(_Type);

}

void Management::AddLayer(int _Index, wstring _LayerKey, shared_ptr<GameObject> _Object)
{
	ObjectMgr->AddLayer(_Index, _LayerKey, _Object);
}

HRESULT Management::SetScene(shared_ptr<Scene> scene)
{
	if (SceneMgr == nullptr)
		return E_FAIL;

	return SceneMgr->SetScene(scene);
}

void Management::Initialize(HINSTANCE hInst, HWND hWnd, int SceneCnt, int screenWidth, int screenHeight, bool vsync, bool fullscreen, float screenDepth, float screenNear)
{
	SceneMgr = SceneManager::GetInstance();
	ObjectMgr = ObjectManager::GetInstance();
	Pipeline = PipeLine::GetInstance();

	GraphicDevice::GetInstance()->Initialize(screenWidth, screenHeight, vsync, hWnd, fullscreen, screenDepth, screenNear);
	InputDevice::GetInstance()->Initialize(hInst, hWnd, screenWidth, screenHeight);

	ObjectMgr->ReserveLayerMap(SceneCnt);
}
