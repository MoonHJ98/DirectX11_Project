#pragma once

#include "Component.h"

class GraphicDevice;
class Management;
class Camera;
class GameObject;

class Transform : public Component, public enable_shared_from_this<Transform>
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
	Vector3						GetState(STATE _State);
	void 						SetState(STATE _State, Vector3 Vec);
	Matrix*						GetWorldMatrix();
	void						SetScale(Vector3 scale);
	Vector3						GetScale();
	void						SetObject(shared_ptr<GameObject> _object);

public:
	// Component을(를) 통해 상속됨
	virtual int Update(float _timeDelta = 0) override;
	virtual void Render() override;
	virtual void RenderInspector() override;
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
	HRESULT						SetRotation(Vector3 angle);
	Vector3						GetRotation() { return rotationAngle; }

public:
	static shared_ptr<Transform>	Create(TRANSDESC _TransDecs);

private:
	shared_ptr<GraphicDevice>		 Graphic = nullptr;
	shared_ptr<Management>			 Manage = nullptr;
	ConstantBuffer<MATRIXBUFFERTYPE> MatrixBuffer;
	Matrix						     WorldMatrix;
	XMFLOAT3					     Rotation;
	TRANSDESC					     Status;
	float						     TimeAcc = 0.f;

	

	Vector3 rotationAngle = Vector3(0.f, 0.f, 0.f);


};

