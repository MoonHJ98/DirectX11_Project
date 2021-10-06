#include "pch.h"
#include "StaticCamera.h"
#include "Management.h"
#include "Transform.h"

StaticCamera::StaticCamera()
{
}

StaticCamera::StaticCamera(const StaticCamera & Rhs)
{
}

StaticCamera::~StaticCamera()
{
}

HRESULT StaticCamera::KeyInput(float _TimeDelta)
{
	auto Manage = Management::GetInstance();

	if (Manage->GetDIKeyState(DIK_W) & 0x80)
	{
		transform->GoForward(_TimeDelta);
	}
	if (Manage->GetDIKeyState(DIK_S) & 0x80)
	{
		transform->GoBackward(_TimeDelta);
	}
	if (Manage->GetDIKeyState(DIK_A) & 0x80)
	{
		transform->GoLeft(_TimeDelta);
	}
	if (Manage->GetDIKeyState(DIK_D) & 0x80)
	{
		transform->GoRight(_TimeDelta);
	}
	if (Manage->GetDIKeyState(DIK_SPACE) & 0x80)
	{
		transform->GoUp(_TimeDelta);
	}
	if (Manage->GetDIKeyState(DIK_LCONTROL) & 0x80)
	{
		transform->GoDown(_TimeDelta);
	}



	return S_OK;
}

HRESULT StaticCamera::MouseInput(float _TimeDelta)
{
	LONG MouseMove = 0;

	if (MouseMove = InputDevice::GetInstance()->GetDIMouseMove(InputDevice::DIMS_X))
	{
		transform->RotationAxis(XMFLOAT3(0.f, 1.f, 0.f), _TimeDelta * (float)MouseMove / 100.f);
	}

	if (MouseMove = InputDevice::GetInstance()->GetDIMouseMove(InputDevice::DIMS_Y))
	{
		XMFLOAT3 Right;
		XMStoreFloat3(&Right, transform->GetState(Transform::RIGHT));

		transform->RotationAxis(Right, _TimeDelta * (float)MouseMove / 100.f);
	}

	return S_OK;
}

int StaticCamera::Update(float _TimeDelta)
{
	KeyInput(_TimeDelta);

	MouseInput(_TimeDelta);
	MouseInput(_TimeDelta);

	XMMATRIX WorldMatrix = *transform->GetWorldMatrix();

	XMFLOAT4X4 TempWorld;
	XMStoreFloat4x4(&TempWorld, WorldMatrix);


	XMVECTOR positionVector = *(XMVECTOR*)&TempWorld.m[Transform::POSITION][0];
	XMVECTOR upVector = *(XMVECTOR*)&TempWorld.m[Transform::UP][0];
	XMVECTOR lookAtVector = *(XMVECTOR*)&TempWorld.m[Transform::LOOK][0];

	lookAtVector = XMVectorAdd(positionVector, lookAtVector);

	UpdateViewMatrix(positionVector, lookAtVector, upVector);

	XMFLOAT3 a = GetPosition();


	return 0;
}

void StaticCamera::Render()
{
}

StaticCamera * StaticCamera::Create(CAMERADECS _Decs)
{
	StaticCamera* Instance = new StaticCamera();

	if (nullptr == Instance || FAILED(Instance->Initialize(_Decs)))
	{
		MSG_BOX("Failed to create StaticCamera.");
		return nullptr;
	}

	return Instance;
}
