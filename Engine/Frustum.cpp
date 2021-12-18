#include "pch.h"
#include "Frustum.h"

Frustum::Frustum()
{
}

Frustum::Frustum(const Frustum & Rhs)
{
}

Frustum::~Frustum()
{
}

int Frustum::Update(float _timeDelta)
{
	return 0;
}

void Frustum::Render()
{
}

void Frustum::UpdateFrustum(float _screenDepth, Matrix& _projectionMatrix, Matrix& _viewMatrix)
{
	// 절두체에서 최소 z거리 계산.
	float zMinimum = -_projectionMatrix._43 / _projectionMatrix._33;
	float r = _screenDepth / (_screenDepth - zMinimum);

	// 업데이트 된 값을 다시 투영 행렬에 설정
	_projectionMatrix._33 = r;
	_projectionMatrix._43 = -r * zMinimum;

	Matrix finalMatrix = _viewMatrix * _projectionMatrix;

	// 절두체의 가까운 평면을 계산
	Vector4 result;
	result.x = (float)(finalMatrix._14 + finalMatrix._13);
	result.y = (float)(finalMatrix._24 + finalMatrix._23);
	result.z = (float)(finalMatrix._34 + finalMatrix._33);
	result.w = (float)(finalMatrix._44 + finalMatrix._43);

	planes[0] = result;
	planes[0] = XMPlaneNormalize(planes[0]);


	// 절두체의 먼 평면 계산
	result.x = (float)(finalMatrix._14 - finalMatrix._13);
	result.y = (float)(finalMatrix._24 - finalMatrix._23);
	result.z = (float)(finalMatrix._34 - finalMatrix._33);
	result.w = (float)(finalMatrix._44 - finalMatrix._43);

	planes[1] = result;
	planes[1] = XMPlaneNormalize(planes[1]);


	// 절두체의 왼쪽 평면 계산
	result.x = (float)(finalMatrix._14 + finalMatrix._11);
	result.y = (float)(finalMatrix._24 + finalMatrix._21);
	result.z = (float)(finalMatrix._34 + finalMatrix._31);
	result.w = (float)(finalMatrix._44 + finalMatrix._41);

	planes[2] = result;
	planes[2] = XMPlaneNormalize(planes[2]);


	// 절두체의 오른쪽 평면 계산
	result.x = (float)(finalMatrix._14 - finalMatrix._11);
	result.y = (float)(finalMatrix._24 - finalMatrix._21);
	result.z = (float)(finalMatrix._34 - finalMatrix._31);
	result.w = (float)(finalMatrix._44 - finalMatrix._41);

	planes[3] = result;
	planes[3] = XMPlaneNormalize(planes[3]);


	// 절두체의 윗 평명 계산
	result.x = (float)(finalMatrix._14 - finalMatrix._12);
	result.y = (float)(finalMatrix._24 - finalMatrix._22);
	result.z = (float)(finalMatrix._34 - finalMatrix._32);
	result.w = (float)(finalMatrix._44 - finalMatrix._42);

	planes[4] = result;
	planes[4] = XMPlaneNormalize(planes[4]);

	// 절두체의 아래 평면 계산
	result.x = (float)(finalMatrix._14 + finalMatrix._12);
	result.y = (float)(finalMatrix._24 + finalMatrix._22);
	result.z = (float)(finalMatrix._34 + finalMatrix._32);
	result.w = (float)(finalMatrix._44 + finalMatrix._42);

	planes[5] = result;
	planes[5] = XMPlaneNormalize(planes[5]);



}

bool Frustum::CheckPoint(Vector3* _pos)
{
	for (int i = 0; i < 6; ++i)
	{
		if (XMVectorGetX(XMPlaneDotCoord(planes[i], Vector4(_pos->x, _pos->y, _pos->z, 1.f))) < 0.f)
			return false;
	}
	return true;
}

bool Frustum::CheckCube(Vector3 _pos, float _radius)
{
	// Check if any one point of the cube is in the view frustum.
	for (int i = 0; i < 6; i++)
	{
		if (XMVectorGetX(XMPlaneDotCoord(planes[i], Vector3(_pos.x - _radius, _pos.y - _radius, _pos.z - _radius))) >= 0.f)
			continue;
		if (XMVectorGetX(XMPlaneDotCoord(planes[i], Vector3(_pos.x + _radius, _pos.y - _radius, _pos.z - _radius))) >= 0.f)
			continue;
		if (XMVectorGetX(XMPlaneDotCoord(planes[i], Vector3(_pos.x - _radius, _pos.y + _radius, _pos.z - _radius))) >= 0.f)
			continue;
		if (XMVectorGetX(XMPlaneDotCoord(planes[i], Vector3(_pos.x + _radius, _pos.y + _radius, _pos.z - _radius))) >= 0.f)
			continue;
		if (XMVectorGetX(XMPlaneDotCoord(planes[i], Vector3(_pos.x - _radius, _pos.y - _radius, _pos.z + _radius))) >= 0.f)
			continue;
		if (XMVectorGetX(XMPlaneDotCoord(planes[i], Vector3(_pos.x + _radius, _pos.y - _radius, _pos.z + _radius))) >= 0.f)
			continue;
		if (XMVectorGetX(XMPlaneDotCoord(planes[i], Vector3(_pos.x - _radius, _pos.y + _radius, _pos.z + _radius))) >= 0.f)
			continue;
		if (XMVectorGetX(XMPlaneDotCoord(planes[i], Vector3(_pos.x + _radius, _pos.y + _radius, _pos.z + _radius))) >= 0.f)
			continue;

		return false;
	}

	return true;
}

bool Frustum::CheckSphere(Vector3 _pos, float _radius)
{
	for (int i = 0; i < 6; ++i)
	{
		if (XMVectorGetX(XMPlaneDotCoord(planes[i], Vector4(_pos.x, _pos.y, _pos.z, 1.f))) < -_radius)
			return true;
	}
	return false;
}

bool Frustum::CheckRectangle(Vector3 _pos, Vector3 _size)
{
	for (int i = 0; i < 6; ++i)
	{
		if (XMVectorGetX(XMPlaneDotCoord(planes[i], Vector4(_pos.x - _size.x, _pos.y - _size.y, _pos.z - _size.z, 1.f))) >= 0.f)
			continue;
		if (XMVectorGetX(XMPlaneDotCoord(planes[i], Vector4(_pos.x + _size.x, _pos.y - _size.y, _pos.z - _size.z, 1.f))) >= 0.f)
			continue;
		if (XMVectorGetX(XMPlaneDotCoord(planes[i], Vector4(_pos.x - _size.x, _pos.y + _size.y, _pos.z - _size.z, 1.f))) >= 0.f)
			continue;
		if (XMVectorGetX(XMPlaneDotCoord(planes[i], Vector4(_pos.x - _size.x, _pos.y - _size.y, _pos.z + _size.z, 1.f))) >= 0.f)
			continue;
		if (XMVectorGetX(XMPlaneDotCoord(planes[i], Vector4(_pos.x + _size.x, _pos.y + _size.y, _pos.z - _size.z, 1.f))) >= 0.f)
			continue;
		if (XMVectorGetX(XMPlaneDotCoord(planes[i], Vector4(_pos.x + _size.x, _pos.y - _size.y, _pos.z + _size.z, 1.f))) >= 0.f)
			continue;
		if (XMVectorGetX(XMPlaneDotCoord(planes[i], Vector4(_pos.x - _size.x, _pos.y + _size.y, _pos.z + _size.z, 1.f))) >= 0.f)
			continue;
		if (XMVectorGetX(XMPlaneDotCoord(planes[i], Vector4(_pos.x + _size.x, _pos.y + _size.y, _pos.z + _size.z, 1.f))) >= 0.f)
			continue;

		return false;
	}

	return true;
}

HRESULT Frustum::Initialize()
{
	return S_OK;
}

shared_ptr<Frustum> Frustum::Create()
{
	shared_ptr<Frustum> Instance(new Frustum());
	if (FAILED(Instance->Initialize()))
	{
		MSG_BOX("Failed to create Frustum.");
		return nullptr;
	}
	return Instance;
}
