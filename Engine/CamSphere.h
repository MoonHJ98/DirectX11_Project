#pragma once
#include "GameObject.h"

class GraphicDevice;
class Sphere;


class CamSphere : public GameObject, public enable_shared_from_this<CamSphere>
{
private:
	CamSphere();
	CamSphere(const CamSphere& Rhs);

public:
	~CamSphere();

public:
	// GameObject을(를) 통해 상속됨
	virtual int Update(float _TimeDelta) override;
	virtual void Render() override;
	virtual void OnContact() override;
	virtual void OnTrigger() override;

private:
	HRESULT Initialize(Vector3 _pos, Vector3 _color);

public:
	static shared_ptr<CamSphere> Create(Vector3 _pos, Vector3 _color = Vector3(-1.f, -1.f, -1.f));

private:
	shared_ptr<Sphere> sphere;
};

