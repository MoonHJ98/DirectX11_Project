#include "pch.h"
#include "StaticCamera.h"
#include "Management.h"
#include "Transform.h"
#include "GraphicDevice.h"
#include "ImguiManager.h"

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

	auto pos = transform->GetState(Transform::POSITION);

	

	return S_OK;
}

HRESULT StaticCamera::MouseInput(float _TimeDelta)
{
	LONG MouseMove = 0;


	if (InputDevice::GetInstance()->GetDIMouseState(InputDevice::DIM_RB) & 0x80)
	{
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
	}

	return S_OK;
}

HRESULT StaticCamera::Initialize(CAMERADECS _Decs)
{
	__super::Initialize(_Decs);

	shared_ptr<ConstantBuffer<CameraBufferType>> temp(new ConstantBuffer<CameraBufferType>());
	CameraBuffer = temp;
	CameraBuffer->Create(Graphic->GetDevice());



	return S_OK;
}

int StaticCamera::Update(float _TimeDelta)
{
	KeyInput(_TimeDelta);

	MouseInput(_TimeDelta);

	XMMATRIX WorldMatrix = *transform->GetWorldMatrix();

	XMFLOAT4X4 TempWorld;
	XMStoreFloat4x4(&TempWorld, WorldMatrix);


	XMVECTOR positionVector = *(XMVECTOR*)&TempWorld.m[Transform::POSITION][0];
	XMVECTOR upVector = *(XMVECTOR*)&TempWorld.m[Transform::UP][0];
	XMVECTOR lookAtVector = *(XMVECTOR*)&TempWorld.m[Transform::LOOK][0];

	lookAtVector = XMVectorAdd(positionVector, lookAtVector);

	UpdateViewMatrix(positionVector, lookAtVector, upVector);


	Vector2 sceneSize = ImguiManager::GetInstance()->GetSceneSize();
	//if (sceneSize.x != 0)
	//{
	//	Decs.ScreenWidth = sceneSize.x;
	//	Decs.ScreenHeight = sceneSize.y;
	//	Decs.ScreenAspect = sceneSize.x / sceneSize.y;
	//}
	UpdateProjectionMatrix(Decs.ScreenWidth, Decs.ScreenHeight, Decs.FiedOfView, Decs.ScreenAspect, Decs.Near, Decs.Far);


	CameraBufferDesc.CameraPosition = positionVector;

	return 0;
}

void StaticCamera::Render()
{
	CameraBuffer->SetData(Graphic->GetDeviceContext(), CameraBufferDesc);
	auto buffer = CameraBuffer->GetBuffer();
	Graphic->GetDeviceContext()->VSSetConstantBuffers(3, 1, &buffer);


}

void StaticCamera::OnContact()
{
}

void StaticCamera::OnTrigger()
{
}

shared_ptr<StaticCamera> StaticCamera::Create(CAMERADECS _Decs)
{
	shared_ptr<StaticCamera> Instance(new StaticCamera);

	if (nullptr == Instance || FAILED(Instance->Initialize(_Decs)))
	{
		MSG_BOX("Failed to create StaticCamera.");
		return nullptr;
	}

	return Instance;
}
