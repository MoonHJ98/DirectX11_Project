#include "pch.h"
#include "Camera.h"
#include "PipeLine.h"
//#include "Management.h"
#include "Transform.h"


Camera::Camera()
{
}


Camera::Camera(const Camera& Rhs)
{
}


Camera::~Camera()
{
}


void Camera::SetPosition(XMFLOAT3 _Position)
{
	transform->SetState(Transform::POSITION, XMLoadFloat3(&_Position));
}


void Camera::SetRotation(XMFLOAT3 _Rotation)
{
	Rotation = _Rotation;
}


XMFLOAT3 Camera::GetPosition()
{
	XMFLOAT3 pos;
	XMStoreFloat3(&pos, transform->GetState(Transform::POSITION));

	return pos;
}


XMFLOAT3 Camera::GetRotation()
{
	return Rotation;
}

HRESULT Camera::UpdateViewMatrix(XMVECTOR _Position, XMVECTOR _LookAt, XMVECTOR _Up)
{
	return pipeline->UpdateViewMatrix(_Position, _LookAt, _Up);
}

HRESULT Camera::UpdateProjectionMatrix(float _FovY, float _Aspect, float _Near, float _Far)
{
	return pipeline->UpdateProjectionMatrix(_FovY, _Aspect, _Near, _Far);
}

HRESULT Camera::Initialize(CAMERADECS _Decs)
{
	Decs = _Decs;
	pipeline = PipeLine::GetInstance();
	pipeline->Initialize();

	transform = Transform::Create(Transform::TRANSDESC(50.f, 5.f));

	UpdateProjectionMatrix(Decs.FiedOfView, Decs.ScreenAspect, Decs.Near, Decs.Far);


	return S_OK;
}