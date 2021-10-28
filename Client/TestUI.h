#pragma once
#include "GameObject.h"

class Trasform;
class Shader;
class RectangleBuffer;
class GraphicDevice;

class TestUI : public GameObject
{
public:
	TestUI();
	~TestUI();

	HRESULT Initialize();



	// GameObject을(를) 통해 상속됨
	virtual int Update(float _TimeDelta) override;

	virtual void Render() override;

	static shared_ptr<TestUI> Create();
	shared_ptr<Shader> shader;
	shared_ptr<RectangleBuffer> rb;
	shared_ptr<GraphicDevice> Graphic;
};

