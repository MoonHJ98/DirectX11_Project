#pragma once

#include "Includes.h"

class Transform
{
public:
	typedef struct tagTransDesc
	{
		tagTransDesc() : Speed(0.f), RotationRadian(0.f) {}
		tagTransDesc(float Speed, float RotationRadian)
		{
			this->Speed = Speed;
			this->RotationRadian = RotationRadian;
		}

		float	Speed = 0.f;
		float  RotationRadian = 0.f;
	}TRANSDESC;

	enum	 STATE { RIGHT, UP, LOOK, POSITION, STATE_END };

private:
	explicit Transform();
	explicit Transform(Transform& Rhs);

public:
	~Transform();

public:
	HRESULT						Initialize(TRANSDESC _TransDecs);
	XMVECTOR					GetState(STATE _State);
	void 						SetState(STATE _State, XMVECTOR Vec);
	XMMATRIX*					GetWorldMatrix();

public:

	HRESULT						GoForward(float _Frametime);
	HRESULT						GoBackward(float _Frametime);
	HRESULT						GoLeft(float _Frametime);
	HRESULT						GoRight(float _Frametime);
	HRESULT						GoUp(float _Frametime, float* _UpMaxDist = nullptr, float* _UpDist = nullptr, bool* _Up = nullptr);
	HRESULT						GoDown(float _Frametime);
	HRESULT						LookTarget(XMFLOAT3 _TargetPos);
	HRESULT						GoToTarget(XMFLOAT3 _TargetPos, float _Frametime);
	HRESULT						RotationAxis(XMFLOAT3 _Axis, float _Frametime, float* _AngleAcc = nullptr);
	HRESULT						SetRotation(XMFLOAT3 _Axis, float _Radian);

public:
	static Transform*		Create(TRANSDESC _TransDecs);

private:
	XMMATRIX					WorldMatrix;
	XMFLOAT3					Rotation;
	TRANSDESC					Status;
	float						TimeAcc = 0.f;
};

