#pragma once

#include "GameObject.h"

class RectangleBuffer;
class Material;

class UI : public GameObject, public enable_shared_from_this<UI>
{
private:
	UI();
	UI(const UI& Rhs);

public:
	~UI();

public:
	// GameObject을(를) 통해 상속됨
	virtual int Update(float _TimeDelta) override;
	virtual void Render() override;
	virtual void OnContact() override;
	virtual void OnTrigger() override;

private:
	HRESULT Initialize();

public:
	static shared_ptr<UI> Create();

private:
	shared_ptr<RectangleBuffer> rectBuffer = nullptr;
	shared_ptr<Material> material;
};

