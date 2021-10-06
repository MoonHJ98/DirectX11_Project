#include "pch.h"
#include "MainApp.h"
#include "Management.h"

#include "GraphicDevice.h"
#include "InputDevice.h"
#include "Stage.h"

MainApp::MainApp()
{
}

MainApp::MainApp(const MainApp & Rhs)
{
}

MainApp::~MainApp()
{
	delete stage;
	stage = nullptr;
}

HRESULT MainApp::Initialize()
{
	Manage = Management::GetInstance();
	GraphicDev = GraphicDevice::GetInstance();
	if (nullptr == GraphicDev)
		return E_FAIL;

	Manage->Initialize(GInstance, GhWnd, SCENE_END, GX, GY, VSYNC_ENABLE, FULLSCREEN, SCREENDEPTH, SCREENNEAR);

	stage = Stage::Create(GraphicDev->GetDevice());

	return S_OK;
}

int MainApp::Update(float _timeDelta)
{
	Manage->UpdateInputDevice();

	Manage->Update(_timeDelta);

	stage->Update(_timeDelta);
	return 0;
}

void MainApp::Render()
{
	GraphicDev->BeginScene(0.f, 0.f, 1.f, 1.f);

	Manage->Render();

	stage->Render();

	GraphicDev->EndScene();
}

MainApp * MainApp::Create()
{
	auto Instance = new MainApp();
	if (FAILED(Instance->Initialize()))
	{
		MSG_BOX("Failed to create MainApp.");
		return nullptr;
	}
	return Instance;
}
