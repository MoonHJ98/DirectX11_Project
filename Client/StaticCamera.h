#pragma once

#include "Camera.h"
class StaticCamera : public Camera
{
private:
	StaticCamera();
	StaticCamera(const StaticCamera& Rhs);

public:
	~StaticCamera();

private:
public:
	HRESULT					KeyInput(float _TimeDelta);
	HRESULT					MouseInput(float _TimeDelta);

public:
	virtual int Update(float _TimeDelta) override;
	virtual void Render() override;

public:
	static shared_ptr<StaticCamera>	Create(CAMERADECS _Decs);

};

