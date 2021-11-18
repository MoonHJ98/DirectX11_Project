#pragma once

#include "GameObject.h"

class Sphere;
class Transform;
class Shader;

class SphereTest : public GameObject, public enable_shared_from_this<SphereTest>
{
private:
	SphereTest();
	SphereTest(const SphereTest& Rhs);

public:
	~SphereTest();

public:
	virtual int Update(float _TimeDelta) override;
	virtual void Render() override;

private:
	HRESULT Initialize();

public:
	static shared_ptr<SphereTest> Create();

private:
	shared_ptr<Shader> shader = nullptr;
	shared_ptr<Sphere> sphere = nullptr;

};
