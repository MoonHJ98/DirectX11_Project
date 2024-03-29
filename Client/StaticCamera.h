#pragma once

#include "Camera.h"

class Frustum;

class StaticCamera : public Camera
{
private:
	StaticCamera();
	StaticCamera(const StaticCamera& Rhs);

public:
	~StaticCamera();

private:
	struct CameraBufferType
	{
		Vector3 CameraPosition;
		float padding = 0.f;
	};


public:
	HRESULT					KeyInput(float _TimeDelta);
	HRESULT					MouseInput(float _TimeDelta);


private:
	HRESULT					Initialize(CAMERADECS _Decs);
	void PlayCam();

public:
	virtual int Update(float _TimeDelta) override;
	virtual void Render() override;
	virtual void OnContact() override;
	virtual void OnTrigger() override;

public:
	static shared_ptr<StaticCamera>	Create(CAMERADECS _Decs);

private:
	CameraBufferType CameraBufferDesc;
	shared_ptr<ConstantBuffer<CameraBufferType>> CameraBuffer;

	bool playCam = false;
	vector<Vector3> eye;
	vector<Vector3> at;

	UINT eyeIndex = 0;
	UINT atIndex;

	Vector3 originPos;
	Vector3 originLook;
	Vector3 originUp;
};

