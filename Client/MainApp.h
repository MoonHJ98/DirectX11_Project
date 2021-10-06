#pragma once

#include "Base.h"

class Management;

class GraphicDevice;
class Stage;

class MainApp : public Base
{
public:
	MainApp();
	MainApp(const MainApp& Rhs);

public:
	~MainApp();

private:
	HRESULT Initialize();

public:
	virtual int Update(float _timeDelta) override;
	virtual void Render() override;

public:
	static MainApp* Create();

private:
	shared_ptr<Management>      Manage = nullptr;
	shared_ptr<GraphicDevice>	GraphicDev = nullptr;
	Stage* stage = nullptr;



};

