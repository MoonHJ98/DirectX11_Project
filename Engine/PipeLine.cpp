#include "pch.h"
#include "PipeLine.h"

HRESULT PipeLine::Initialize()
{
	ViewMatrix = ProjectionMatrix = XMMatrixIdentity();
	return E_NOTIMPL;
}

Matrix* PipeLine::GetTransform(_D3DTRANSFORMSTATETYPE _Type)
{
	switch (_Type)
	{
	case D3DTRANSFORMSTATE_VIEW:
		return &ViewMatrix;

	case D3DTRANSFORMSTATE_PROJECTION:
		return &ProjectionMatrix;

	default:
		return nullptr;
	}

	return nullptr;
}

HRESULT PipeLine::UpdateViewMatrix(XMVECTOR _PositionVector, XMVECTOR _LookAtVector, XMVECTOR _UpVector)
{
	ViewMatrix = XMMatrixLookAtLH(_PositionVector, _LookAtVector, _UpVector);

	return S_OK;
}

HRESULT PipeLine::UpdateProjectionMatrix(float _FovY, float _Aspect, float _Near, float _Far)
{
	ProjectionMatrix = XMMatrixPerspectiveFovLH(_FovY, _Aspect, _Near, _Far);
	return S_OK;
}