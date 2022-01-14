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

public:
	bool& GetPlayButtonPressed() { return playButtonPressed; }
	void SetPlayButtonPressed(bool _playButton) { playButtonPressed = _playButton; }
	void GetEyeAtVector(vector<Vector3>& _eye, vector<Vector3>& _at);

private:
	HRESULT Initialize(shared_ptr<GameObject> _object);
	void CamList();
	void AddCam();
	void EyeVectors();
	void AtVectors();
	void RenderSpheres();
	void RenderLine();
	void UpdateCamPos(vector<Vector3>* _eyePos, vector<Vector3>* _atPos);
	void UpdateLine(shared_ptr<Line>* _eyeLine, shared_ptr<Line>* _atLine, vector<Vector3>& pos, Vector4 _color);
	void UpdateCamSphere(vector<shared_ptr<CamSphere>>* _eyeSphere, vector<shared_ptr<CamSphere>>* _atSphere, bool _addSphere = true, Vector3 _color = Vector3(1.f, 1.f, 1.f));
	void PlayCamera();

public:
	static shared_ptr<CameraComponent> Create(shared_ptr<GameObject> _object);

private:
	// pair<eye, at>
	unordered_map<string, pair<vector<Vector3>, vector<Vector3>>> camList;
	unordered_map<string, pair<vector<Vector3>, vector<Vector3>>> EyeAtList;
	unordered_map<string, pair<vector<shared_ptr<CamSphere>>, vector<shared_ptr<CamSphere>>>> camSpheres;
	unordered_map<string, pair<shared_ptr<Line>, shared_ptr<Line>>> lineList;

	int camCurrentIndex = 0;
	int eyeCurrentIndex = 0;
	int atCurrentIndex = 0;

	bool rander = false;
	bool playButtonPressed = false;
};

