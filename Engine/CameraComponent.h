#pragma once
#include "Component.h"

class GameObject;
class CamSphere;
class Line;

class CameraComponent : public Component
{
private:
	CameraComponent();
	CameraComponent(const CameraComponent& Rhs);

public:
	~CameraComponent();

public:
	// Component을(를) 통해 상속됨
	virtual int Update(float _timeDelta) override;
	virtual void Render() override;
	virtual void RenderInspector() override;


private:
	HRESULT Initialize(shared_ptr<GameObject> _object);
	void CamList();
	void AddCam();
	void EyeVectors();
	void AtVectors();
	void RenderSpheres();
	

public:
	static shared_ptr<CameraComponent> Create(shared_ptr<GameObject> _object);

private:
	unordered_map<string, pair<vector<Vector3>, vector<Vector3>>> camList;

	unordered_map<string, pair<vector<shared_ptr<CamSphere>>, vector<shared_ptr<CamSphere>>>> camSpheres;

	int camCurrentIndex = 0;
	int eyeCurrentIndex = 0;
	int atCurrentIndex = 0;

	bool randerSpheres = false;

	shared_ptr<Line> line = nullptr;
};

