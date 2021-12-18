#pragma once

#include "Base.h"

class Frustum : public Base
{
private:
	Frustum();
	Frustum(const Frustum& Rhs);

public:
	~Frustum();

public:
	// Base을(를) 통해 상속됨
	virtual int Update(float _timeDelta = 0) override;
	virtual void Render() override;

public:
	void UpdateFrustum(float _screenDepth, Matrix& _projectionMatrix, Matrix& _viewMatrix);

	bool CheckPoint(Vector3* _pos);
	bool CheckCube(Vector3 _pos, float _radius);
	bool CheckSphere(Vector3 _pos, float _radius);
	bool CheckRectangle(Vector3 _pos, Vector3 _size);

private:
	HRESULT Initialize();

public:
	static shared_ptr<Frustum> Create();

private:
	Vector4 planes[6];

};

