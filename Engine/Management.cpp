#include "pch.h"
#include "Management.h"
#include "GraphicDevice.h"
#include "SceneManager.h"
#include "ObjectManager.h"

int Management::Update(float _timeDelta)
{
	SceneMgr->Update(_timeDelta);

	ObjectMgr->Update(_timeDelta);

	// test 1

   // test 2

	//test 3

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

void Management::Initialize(HINSTANCE hInst, HWND hWnd, int SceneCnt, int screenWidth, int screenHeight, bool vsync, bool fullscreen, float screenDepth, float screenNear)
{
	SceneMgr = SceneManager::GetInstance();
	ObjectMgr = ObjectManager::GetInstance();

	GraphicDevice::GetInstance()->Initialize(screenWidth, screenHeight, vsync, hWnd, fullscreen, screenDepth, screenNear);
	InputDevice::GetInstance()->Initialize(hInst, hWnd, screenWidth, screenHeight);

	ObjectMgr->ReserveLayerMap(SceneCnt);
}
