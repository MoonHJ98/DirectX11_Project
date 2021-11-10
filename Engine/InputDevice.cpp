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

	
	// DInput �İ�ü�� �����ϴ� �Լ�
	if (FAILED((DirectInput8Create(hInstance,
		DIRECTINPUT_VERSION,
		IID_IDirectInput8,
		(void**)&DirectInput,
		NULL))))
		return E_FAIL;
	
	// Ű���� ��ü ����
	if (FAILED(DirectInput->CreateDevice(GUID_SysKeyboard, &Keyboard, nullptr)))
		return E_FAIL;
	
	// ������ Ű���� ��ü�� ���� ������ �� ��ü���� �����ϴ� �Լ�
	Keyboard->SetDataFormat(&c_dfDIKeyboard);
	
	// ��ġ�� ���� �������� �������ִ� �Լ�, Ŭ���̾�Ʈ�� ���ִ� ���¿��� Ű �Է��� ������ ������ �����ϴ� �Լ�
	Keyboard->SetCooperativeLevel(hWnd, DISCL_BACKGROUND | DISCL_NONEXCLUSIVE);

	// ��ġ�� ���� access ������ �޾ƿ��� �Լ�
	Keyboard->Acquire();

	// ���콺 ��ü ����
	if (FAILED(DirectInput->CreateDevice(GUID_SysMouse, &mouse, nullptr)))
		return E_FAIL;

	// ������ ���콺 ��ü�� ���� ������ �� ��ü���� �����ϴ� �Լ�
	if (FAILED(mouse->SetDataFormat(&c_dfDIMouse)))
		return E_FAIL;

	// ��ġ�� ���� �������� �������ִ� �Լ�, Ŭ���̾�Ʈ�� ���ִ� ���¿��� Ű �Է��� ������ ������ �����ϴ� �Լ�
	if (FAILED(mouse->SetCooperativeLevel(hWnd, DISCL_BACKGROUND | DISCL_NONEXCLUSIVE)))
		return E_FAIL;

	// ��ġ�� ���� access ������ �޾ƿ��� �Լ�
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

