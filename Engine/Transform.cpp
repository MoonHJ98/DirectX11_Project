#include "pch.h"
#include "Transform.h"
#include "GraphicDevice.h"
#include "Management.h"
#include "Camera.h"
#include "Light.h"
#include "GameObject.h"

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
	componentType = TRANSFORM;
	Graphic = GraphicDevice::GetInstance();
	Manage = Management::GetInstance();

	MatrixBuffer.Create(Graphic->GetDevice());

	Status = _TransDecs;
	WorldMatrix = XMMatrixIdentity();
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

void Transform::SetScale(Vector3 scale)
{
	Vector3 Right = GetState(RIGHT);
	Vector3 Up = GetState(UP);
	Vector3 Look = GetState(LOOK);

	XMVECTOR vecRight = XMLoadFloat3(&Right);
	XMVECTOR vecUp = XMLoadFloat3(&Up);
	XMVECTOR vecLook = XMLoadFloat3(&Look);

	vecRight = XMVector3Normalize(vecRight) * scale.x;
	vecUp = XMVector3Normalize(vecUp) * scale.y;
	vecLook = XMVector3Normalize(vecLook) * scale.z;

	XMStoreFloat3(&Right, vecRight);
	XMStoreFloat3(&Up, vecUp);
	XMStoreFloat3(&Look, vecLook);


	SetState(RIGHT, Right);
	SetState(UP, Up);
	SetState(LOOK, Look);

}

Vector3 Transform::GetScale()
{
	Vector3 right = GetState(RIGHT);
	Vector3 up = GetState(UP);
	Vector3 look = GetState(LOOK);


	return Vector3(right.Length(), up.Length(), look.Length());
}

void Transform::SetObject(shared_ptr<GameObject> _object)
{
	object = _object;
}

//void Transform::Update(bool isOrtho, BOOL _depthForShadow)
//{
//	MATRIXBUFFERTYPE desc;
//	desc.World = XMMatrixTranspose(WorldMatrix);
//	desc.View = XMMatrixTranspose(*Manage->GetTransform(D3DTRANSFORMSTATE_VIEW));
//
//	if (isOrtho)
//	{
//		Matrix View = XMMatrixIdentity();
//		desc.World = XMMatrixIdentity();
//
//		Vector3 scale = GetScale();
//		View(0, 0) = scale.x;
//		View(1, 1) = scale.y;
//
//		Vector3 pos = GetState(POSITION);
//		View(3, 0) = pos.x;
//		View(3, 1) = pos.y;
//		View(3, 2) = pos.z;
//
//		desc.View = XMMatrixTranspose(View);
//		desc.Projection = XMMatrixTranspose(*Manage->GetTransform(D3DTRANSFORMSTATE_TEXTURE0));
//	}
//	else
//		desc.Projection = XMMatrixTranspose(*Manage->GetTransform(D3DTRANSFORMSTATE_PROJECTION));
//
//	auto light = Manage->FindLight(L"DirectionalLight", 0);
//	Matrix lightViewMatrix = *light->GetViewMatrix();
//	Matrix lightProjectionMatrix = *light->GetOrthoMatrix();
//
//	if (_depthForShadow == TRUE)
//	{
//		desc.View = XMMatrixTranspose(lightViewMatrix);
//		desc.Projection = XMMatrixTranspose(lightProjectionMatrix);
//	}
//
//	desc.lightViewMatrix = XMMatrixTranspose(lightViewMatrix);
//	desc.lightProjectionMatrix = XMMatrixTranspose(lightProjectionMatrix);
//
//
//	desc.renderDepthForShadow = _depthForShadow;
//	MatrixBuffer.SetData(Graphic->GetDeviceContext(), desc);
//	auto buffer = MatrixBuffer.GetBuffer();
//	Graphic->GetDeviceContext()->VSSetConstantBuffers(0, 1, &buffer);
//}

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

HRESULT Transform::SetRotation(Vector3 angle)
{
	Vector3 Right(1.f, 0.f, 0.f), Look(0.f, 0.f, 1.f), Up(0.f, 1.f, 0.f);
	rotationAngle = angle;

	Matrix matRot;

	Right *= GetScale().x;
	Up *= GetScale().y;
	Look *= GetScale().z;

	Matrix rotX = XMMatrixRotationX(XMConvertToRadians(rotationAngle.x));
	Matrix rotY = XMMatrixRotationY(XMConvertToRadians(rotationAngle.y));
	Matrix rotZ = XMMatrixRotationZ(XMConvertToRadians(rotationAngle.z));



	matRot = rotX * rotY * rotZ;

	Right = XMVector3TransformNormal(Right, matRot);
	Look = XMVector3TransformNormal(Look, matRot);
	Up = XMVector3TransformNormal(Up, matRot);

	SetState(Transform::RIGHT, Right);
	SetState(Transform::LOOK, Look);
	SetState(Transform::UP, Up);

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

int Transform::Update(float _timeDelta)
{
	return 0;
}

void Transform::Render()
{
	auto obj = object.lock();

	if (obj == nullptr)
	{
		MATRIXBUFFERTYPE desc;
		desc.World = XMMatrixTranspose(WorldMatrix);
		Matrix View = XMMatrixIdentity();
		desc.World = XMMatrixIdentity();

		Vector3 scale = GetScale();
		View(0, 0) = scale.x;
		View(1, 1) = scale.y;

		Vector3 pos = GetState(POSITION);
		View(3, 0) = pos.x;
		View(3, 1) = pos.y;
		View(3, 2) = pos.z;

		desc.View = XMMatrixTranspose(View);
		desc.Projection = XMMatrixTranspose(*Manage->GetTransform(D3DTRANSFORMSTATE_TEXTURE0));
		MatrixBuffer.SetData(Graphic->GetDeviceContext(), desc);
		auto buffer = MatrixBuffer.GetBuffer();
		Graphic->GetDeviceContext()->VSSetConstantBuffers(0, 1, &buffer);

		return;
	}

	MATRIXBUFFERTYPE desc;
	desc.World = XMMatrixTranspose(WorldMatrix);
	desc.View = XMMatrixTranspose(*Manage->GetTransform(D3DTRANSFORMSTATE_VIEW));

	if (obj->GetisOrth())
	{
		Matrix View = XMMatrixIdentity();
		desc.World = XMMatrixIdentity();

		Vector3 scale = GetScale();
		View(0, 0) = scale.x;
		View(1, 1) = scale.y;

		Vector3 pos = GetState(POSITION);
		View(3, 0) = pos.x;
		View(3, 1) = pos.y;
		View(3, 2) = pos.z;

		desc.View = XMMatrixTranspose(View);
		desc.Projection = XMMatrixTranspose(*Manage->GetTransform(D3DTRANSFORMSTATE_TEXTURE0));
	}
	else
		desc.Projection = XMMatrixTranspose(*Manage->GetTransform(D3DTRANSFORMSTATE_PROJECTION));

	auto light = Manage->FindLight(L"DirectionalLight", 0);
	Matrix lightViewMatrix = *light->GetViewMatrix();
	Matrix lightProjectionMatrix = *light->GetOrthoMatrix();

	if (obj->GetRenderDeptehForShadow() == TRUE)
	{
		desc.View = XMMatrixTranspose(lightViewMatrix);
		desc.Projection = XMMatrixTranspose(lightProjectionMatrix);
	}

	desc.lightViewMatrix = XMMatrixTranspose(lightViewMatrix);
	desc.lightProjectionMatrix = XMMatrixTranspose(lightProjectionMatrix);


	desc.renderDepthForShadow = obj->GetRenderDeptehForShadow();
	MatrixBuffer.SetData(Graphic->GetDeviceContext(), desc);
	auto buffer = MatrixBuffer.GetBuffer();
	Graphic->GetDeviceContext()->VSSetConstantBuffers(0, 1, &buffer);
}

void Transform::RenderInspector()
{
	if (ImGui::CollapsingHeader("Transform"))
	{


		float pos[3];
		pos[0] = GetState(POSITION).x;
		pos[1] = GetState(POSITION).y;
		pos[2] = GetState(POSITION).z;

		ImGui::InputFloat3("Position", pos);
		SetState(POSITION, Vector3(pos[0], pos[1], pos[2]));

		float rot[3];
		rot[0] = GetRotation().x;
		rot[1] = GetRotation().y;
		rot[2] = GetRotation().z;

		ImGui::InputFloat3("Rotation", rot);
		SetRotation(Vector3(rot[0], rot[1], rot[2]));


		float scale[3];
		scale[0] = GetScale().x;
		scale[1] = GetScale().y;
		scale[2] = GetScale().z;

		ImGui::InputFloat3("Scale", scale);
		if (scale[0] != 0 && scale[1] != 0 && scale[2] != 0)
			SetScale(Vector3(scale[0], scale[1], scale[2]));
	}

}
