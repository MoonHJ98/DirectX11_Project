#pragma once

#include "Includes.h"

#include <dinput.h>
#include <DirectXTK/Mouse.h>

class InputDevice
{
	SINGLETON(InputDevice)

public:
	enum MOUSEKEYSTATE { DIM_LB, DIM_RB, DIM_WB, DIM_END };
	enum MOUSEMOVESTATE { DIMS_X, DIMS_Y, DIMS_Z, DIMS_END };

	~InputDevice();

public:
	HRESULT					Initialize(HINSTANCE hInstance, HWND hWnd, int _ScreenWidth, int _ScreenHeight);

	void					UpdateInputDev();



public:
	UCHAR					GetDIKeyState(UCHAR KeyID);
	UCHAR					GetDIMouseState(MOUSEKEYSTATE _MouseState);
	LONG					GetDIMouseMove(MOUSEMOVESTATE _MouseState);




private:
	ComPtr<IDirectInput8>			DirectInput = nullptr;
	ComPtr<IDirectInputDevice8>	    Keyboard = nullptr;
	ComPtr<IDirectInputDevice8>		mouse = nullptr;



	UCHAR					KeyboardState[256];
	DIMOUSESTATE			MouseState;
};

