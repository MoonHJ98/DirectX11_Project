#include "pch.h"
#include "Transform.h"
#include "GraphicDevice.h"
#include "Management.h"

Transform::Transform()
{
	WorldMatrix = XMMatrixIdentity();
}

Transform::Transform(Transform & Rhs)
{
}

Transform::~Transform()
{
}

HRESULT Transform::Initialize(TRANSDESC _TransDecs)
{
	Graphic = GraphicDevice::GetInstance();
	Manage = Management::GetInstance();

	MatrixBuffer.Create(Graphic->GetDevice());

	Status = _TransDecs;
	return S_OK;
}

Vector3 Transform::GetState(STATE _State)
{
	XMFLOAT4X4 temp;
	XMStoreFloat4x4(&temp, WorldMatrix);


	return *(XMVECTOR*)&temp.m[_State][0];
}

void Transform::SetState(STATE _State, Vector3 _Vec)
{
	XMFLOAT4X4 temp;
	XMStoreFloat4x4(&temp, WorldMatrix);

	memcpy(&temp.m[_State][0], &_Vec, sizeof(Vector3));

	WorldMatrix = XMLoadFloat4x4(&temp);
}

Matrix* Transform::GetWorldMatrix()
{
	return &WorldMatrix;
}

void Transform::Update(bool isOrtho, bool isBillboard)
{
	MATRIXBUFFERTYPE desc;
	desc.World = XMMatrixTranspose(WorldMatrix);
	desc.View = XMMatrixTranspose(*Manage->GetTransform(D3DTRANSFORMSTATE_VIEW));

	if(isOrtho)
		desc.Projection = XMMatrixTranspose(*Manage->GetTransform(D3DTRANSFORMSTATE_TEXTURE0));
	else
		desc.Projection = XMMatrixTranspose(*Manage->GetTransform(D3DTRANSFORMSTATE_PROJECTION));


	MatrixBuffer.SetData(Graphic->GetDeviceContext(), desc);
	auto buffer = MatrixBuffer.GetBuffer();
	Graphic->GetDeviceContext()->VSSetConstantBuffers(0, 1, &buffer);
}

HRESULT Transform::GoForward(float _Frametime)
{
	XMVECTOR vLook = GetState(Transform::LOOK);
	XMVECTOR vPos = GetState(Transform::POSITION);

	
	vPos += XMVector3Normalize(vLook) * Status.Speed * static_cast<float>(_Frametime);
	
	SetState(Transform::POSITION, vPos);

	return S_OK;
}

HRESULT Transform::GoBackward(float _Frametime)
{
	XMVECTOR vLook = GetState(Transform::LOOK);
	XMVECTOR vPos = GetState(Transform::POSITION);


	vPos -= XMVector3Normalize(vLook) * Status.Speed * static_cast<float>(_Frametime);

	SetState(Transform::POSITION, vPos);

	return S_OK;
}

HRESULT Transform::GoLeft(float _Frametime)
{
	XMVECTOR vRight = GetState(Transform::RIGHT);
	XMVECTOR vPos = GetState(Transform::POSITION);

	vPos -= XMVector3Normalize(vRight) * Status.Speed * static_cast<float>(_Frametime);

	SetState(Transform::POSITION, vPos);

	return S_OK;
}

HRESULT Transform::GoRight(float _Frametime)
{
	XMVECTOR vRight = GetState(Transform::RIGHT);
	XMVECTOR vPos = GetState(Transform::POSITION);

	vPos += XMVector3Normalize(vRight) * Status.Speed * static_cast<float>(_Frametime);

	SetState(Transform::POSITION, vPos);

	return S_OK;
}

HRESULT Transform::GoUp(float _Frametime, float * UpMaxDist, float * UpDist, bool * Up)
{
	XMVECTOR vLook = GetState(Transform::UP);
	XMVECTOR vPos = GetState(Transform::POSITION);

	if (nullptr != UpMaxDist)
	{
		*UpDist += Status.Speed * static_cast<float>(_Frametime);

		if (*UpMaxDist <= *UpDist)
		{
			*Up = false;
			*UpDist = 0.f;
		}
	}

	vPos += XMVector3Normalize(vLook) * Status.Speed * static_cast<float>(_Frametime);

	SetState(Transform::POSITION, vPos);
	return S_OK;
}

HRESULT Transform::GoDown(float _Frametime)
{
	XMVECTOR vPos = GetState(Transform::POSITION);
	XMFLOAT3 up = { 0.f, 1.f, 0.f };


	vPos += XMVECTOR(XMLoadFloat3(&up)) * Status.Speed * static_cast<float>(_Frametime) * -1;

	SetState(Transform::POSITION, vPos);

	return S_OK;
}

HRESULT Transform::LookTarget(XMFLOAT3 TargetPos)
{
	return S_OK;
}

HRESULT Transform::GoToTarget(XMFLOAT3 TargetPos, float _Frametime)
{
	return S_OK;
}

HRESULT Transform::RotationAxis(XMFLOAT3 _Axis, float _Frametime, float* _AngleAcc)
{


	XMVECTOR vRight, vLook, vUp;
	XMMATRIX matRot = {};

	vRight = GetState(Transform::RIGHT);
	vLook = GetState(Transform::LOOK);
	vUp = GetState(Transform::UP);


	XMVECTOR axis = XMLoadFloat3(&_Axis);

	matRot = XMMatrixRotationAxis(axis, Status.RotationRadian * static_cast<float>(_Frametime));
	vRight = XMVector3TransformNormal(vRight, matRot);
	vLook = XMVector3TransformNormal(vLook, matRot);
	vUp = XMVector3TransformNormal(vUp, matRot);

	SetState(Transform::RIGHT, vRight);
	SetState(Transform::LOOK, vLook);
	SetState(Transform::UP, vUp);

	
	return S_OK;
}

HRESULT Transform::SetRotation(XMFLOAT3 Axis, float Radian)
{
	return S_OK;
}



shared_ptr<Transform> Transform::Create(TRANSDESC _TransDecs)
{
	shared_ptr<Transform> Instance(new Transform());

	if (FAILED(Instance->Initialize(_TransDecs)))
	{
		MSG_BOX("Failed to create Transform.");
		return nullptr;
	}

	return Instance;
}
