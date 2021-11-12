#include "pch.h"
#include "MainApp.h"
#include "Management.h"
#include "StaticCamera.h"
#include "Player.h"
#include "GraphicDevice.h"
#include "Scene.h"
#include "Stage.h"
#include "Terrain.h"
#include "ImguiManager.h"


MainApp::MainApp()
{
}

MainApp::MainApp(const MainApp & Rhs)
{
}

MainApp::~MainApp()
{

}

HRESULT MainApp::Initialize()
{
	Manage = Management::GetInstance();
	GraphicDev = GraphicDevice::GetInstance();
	if (nullptr == GraphicDev)
		return E_FAIL;

	Manage->Initialize(GInstance, GhWnd, SCENE_END, GX, GY, VSYNC_ENABLE, FULLSCREEN, SCREENDEPTH, SCREENNEAR);


	//Light
	LIGHTDESC lightDesc;
	lightDesc.Type = D3DLIGHT_DIRECTIONAL;
	lightDesc.Diffuse = Color(1.f, 1.f, 1.f, 1.f);
	lightDesc.Specular = Color(0.5f, 0.5f, 0.5f, 1.f);
	lightDesc.Ambient = Color(0.4f, 0.4f, 0.4f, 1.f);
	lightDesc.Direction = Vector3(0.5f, -0.5f, 1.f);
	lightDesc.SpecularPower = 5.f;


	Manage->AddLight(lightDesc, L"DirectionalLight");


	Camera::CAMERADECS _decs;
	_decs.Far = SCREENDEPTH;
	_decs.Near = SCREENNEAR;
	_decs.FiedOfView = FoV;
	_decs.ScreenAspect = (float)GX / (float)GY;
	_decs.ScreenWidth = (float)GX;
	_decs.ScreenHeight = (float)GY;

	auto camera = StaticCamera::Create(_decs);
	Manage->AddLayer(SCENEID::STATIC, L"Camera", camera);

	auto player = Player::Create(GraphicDev->GetDevice());
	Manage->AddLayer(SCENEID::STATIC, L"Player", player);

	auto terrain = Terrain::Create(100, 100, L"../Resources/heightmap.bmp");
	Manage->AddLayer(SCENEID::STATIC, L"Terrain", terrain);



	if (FAILED(ReadyScene(LOGO)))
	{
		MSG_BOX("Failed to create Scene.");
		return E_FAIL;
	}

	imguiManager = ImguiManager::GetInstance();;
	imguiManager->Initialize();
	return S_OK;
}

HRESULT MainApp::ReadyScene(SCENEID sceneID)
{
	shared_ptr<Scene> scene = nullptr;
	switch (sceneID)
	{
	case STATIC:
		break;
	case LOGO:
		scene = Stage::Create(GraphicDev->GetDevice());
		GSceneID = LOGO;
		break;
	case SCENE_END:
		break;
	default:
		break;
	}

	if (scene == nullptr)
		return E_FAIL;

	Manage->SetScene(scene);

	return S_OK;
}

int MainApp::Update(float _timeDelta)
{
	Manage->UpdateInputDevice();
	
	Manage->Update(_timeDelta);

	return 0;
}

void MainApp::Render()
{
	GraphicDev->BeginScene(0.f, 0.f, 1.f, 1.f);

	Manage->Render();

	imguiManager->Render();
	//// Start ImGui frame.
	//ImGui_ImplDX11_NewFrame();
	//ImGui_ImplWin32_NewFrame();
	//
	//ImGui::NewFrame();
	//
	//static int counter = 0;
	//// Create ImGui Test Window.
	//ImGui::Begin("Test");
	//ImGui::Text("This is example text.");
	//if (ImGui::Button("Click me"))
	//	counter += 1;
	//
	//string clickCount = "Click Count : " + to_string(counter);
	//ImGui::SameLine();
	//ImGui::Text(clickCount.c_str());
	//
	//ImGui::End();
	//
	//// Assemble Together Draw Data
	//ImGui::Render();
	//// Render Draw Data
	//ImGui_ImplDX11_RenderDrawData(ImGui::GetDrawData());

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
