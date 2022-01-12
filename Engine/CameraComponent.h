#pragma once
#include "Component.h"
class CameraComponent : public Component
{
private:
	CameraComponent();
	CameraComponent(const CameraComponent& Rhs);

public:
	~CameraComponent();

public:
	// Component��(��) ���� ��ӵ�
	virtual int Update(float _timeDelta) override;
	virtual void Render() override;
	virtual void RenderInspector() override;


private:
	HRESULT Initialize();

public:
	static shared_ptr<CameraComponent> Create();

private:
	vector<Vector3> eyePos;
	vector<Vector3> atPos;
};

