#include "pch.h"
#include "InputDevice.h"

InputDevice::~InputDevice()
{
}

void InputDevice::UpdateInputDev()
{
	Keyboard->GetDeviceState(256, KeyboardState);
	mouse->GetDeviceState(sizeof(MouseState), &MouseState);

}

HRESULT InputDevice::Initialize(HINSTANCE hInstance, HWND hWnd, int _ScreenWidth, int _ScreenHeight)
{
	tkmouse = make_unique<Mouse>();
	tkmouse->SetWindow(hWnd);
	tkmouse->SetMode(Mouse::MODE_RELATIVE);
	
	auto state = tkmouse->GetState();

	
	// DInput 컴객체를 생성하는 함수
	if (FAILED((DirectInput8Create(hInstance,
		DIRECTINPUT_VERSION,
		IID_IDirectInput8,
		(void**)&DirectInput,
		NULL))))
		return E_FAIL;
	
	// 키보드 객체 생성
	if (FAILED(DirectInput->CreateDevice(GUID_SysKeyboard, &Keyboard, nullptr)))
		return E_FAIL;
	
	// 생성된 키보드 객체의 대한 정보를 컴 객체에게 전달하는 함수
	Keyboard->SetDataFormat(&c_dfDIKeyboard);
	
	// 장치에 대한 독점권을 설정해주는 함수, 클라이언트가 떠있는 상태에서 키 입력을 받을지 말지를 결정하는 함수
	Keyboard->SetCooperativeLevel(hWnd, DISCL_BACKGROUND | DISCL_NONEXCLUSIVE);

	// 장치에 대한 access 버전을 받아오는 함수
	Keyboard->Acquire();

	// 마우스 객체 생성
	if (FAILED(DirectInput->CreateDevice(GUID_SysMouse, &mouse, nullptr)))
		return E_FAIL;

	// 생성된 마우스 객체의 대한 정보를 컴 객체에게 전달하는 함수
	if (FAILED(mouse->SetDataFormat(&c_dfDIMouse)))
		return E_FAIL;

	// 장치에 대한 독점권을 설정해주는 함수, 클라이언트가 떠있는 상태에서 키 입력을 받을지 말지를 결정하는 함수
	if (FAILED(mouse->SetCooperativeLevel(hWnd, DISCL_BACKGROUND | DISCL_NONEXCLUSIVE)))
		return E_FAIL;

	// 장치에 대한 access 버전을 받아오는 함수
	if (FAILED(mouse->Acquire()))
		return E_FAIL;
	
	return S_OK;

}


UCHAR InputDevice::GetDIKeyState(UCHAR KeyID)
{
	return KeyboardState[KeyID];
}

UCHAR InputDevice::GetDIMouseState(MOUSEKEYSTATE _MouseState)
{
	return MouseState.rgbButtons[_MouseState];
}

LONG InputDevice::GetDIMouseMove(MOUSEMOVESTATE _MouseState)
{
	return *(((LONG*)&MouseState) + _MouseState);
}

