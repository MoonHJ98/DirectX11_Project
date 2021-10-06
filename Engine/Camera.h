#pragma once

#include "GameObject.h"

class PipeLine;
class Transform;

class Camera : public GameObject
{
public:
	typedef struct CameraDecs
	{
		float FiedOfView = 0.f;
		float Near = 0.f;
		float Far = 0.f;
		float ScreenAspect = 0.f;
	}CAMERADECS;
public:
	Camera();
	Camera(const Camera& RhsS);
	~Camera();


public:
	virtual int						Update(float _TimeDelta) PURE;
	virtual void					Render() PURE;

public:
	void							SetPosition(XMFLOAT3 _Position);
	void							SetRotation(XMFLOAT3 _Rotation);

	XMFLOAT3						GetPosition();
	XMFLOAT3						GetRotation();

	HRESULT							UpdateViewMatrix(XMVECTOR _Position, XMVECTOR _LookAt, XMVECTOR _Up);
	HRESULT							UpdateProjectionMatrix(float _FovY, float _Aspect, float _Near, float _Far);

protected:
	HRESULT							Initialize(CAMERADECS _Decs);


protected:
	CAMERADECS						Decs;
	XMFLOAT3						Rotation = {};

	shared_ptr<PipeLine>		    pipeline = nullptr;
};