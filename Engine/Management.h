#pragma once

#include "Base.h"
#include "Includes.h"
#include "InputDevice.h"


class GraphicDevice;

class SceneManager;
class ObjectManager;

class Management : public Base
{
	SINGLETON(Management)

public:
	virtual int  Update(float _timeDelta) override;
	virtual void Render() override;

public:

	// Input Device
	void					UpdateInputDevice();
	UCHAR					GetDIKeyState(UCHAR KeyID);
	UCHAR					GetDIMouseState(InputDevice::MOUSEKEYSTATE _MouseState);
	LONG					GetDIMouseMove(InputDevice::MOUSEMOVESTATE _MouseState);



public:
	void		 Initialize(HINSTANCE hInst, HWND hWnd, int SceneCnt, int screenWidth, int screenHeight, bool vsync, bool fullscreen, float screenDepth, float screenNear);

private:
	shared_ptr<SceneManager>		SceneMgr = nullptr;
	shared_ptr<ObjectManager>		ObjectMgr = nullptr;
};

