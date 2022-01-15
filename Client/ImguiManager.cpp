#include "pch.h"
#include "ImguiManager.h"
#include "GraphicDevice.h"
#include "Renderer.h"
#include "Management.h"
#include "light.h"
#include "Terrain.h"
#include "SphereTest.h"
#include "GameObject.h"
#include "Rigidbody.h"
#include "Collider.h"
#include "UI.h"

HRESULT ImguiManager::Initialize()
{
	Manage = Management::GetInstance();

	IMGUI_CHECKVERSION();
	ImGui::CreateContext();


	ImGui::GetIO().ConfigFlags |= ImGuiConfigFlags_DockingEnable;
	ImGui::GetIO().ConfigFlags |= ImGuiConfigFlags_ViewportsEnable;
	ImGui::GetIO().ConfigFlags |= ImGuiConfigFlags_NavEnableSetMousePos;

	ImGui::GetIO().ConfigWindowsMoveFromTitleBarOnly = true;

	ImGui_ImplWin32_Init(GhWnd);
	ImGui_ImplDX11_Init(GraphicDevice::GetInstance()->GetDevice(), GraphicDevice::GetInstance()->GetDeviceContext());
	ImGui::StyleColorsDark();


	return S_OK;
}

void ImguiManager::Render()
{


	// Start ImGui frame. 갱신되게 하는 것.
	ImGui_ImplDX11_NewFrame();
	ImGui_ImplWin32_NewFrame();

	ImGui::NewFrame();

	//ImGui::DockSpaceOverViewport(ImGui::GetMainViewport());

	Scene();
	Menu1();
	Hierarchy();
	Inspector();




	// Assemble Together Draw Data
	ImGui::Render();
	// Render Draw Data
	ImGui_ImplDX11_RenderDrawData(ImGui::GetDrawData());

	if (ImGui::GetIO().ConfigFlags & ImGuiConfigFlags_ViewportsEnable)
	{
		ImGui::UpdatePlatformWindows();
		ImGui::RenderPlatformWindowsDefault();

	}
}

void ImguiManager::Scene()
{
	static int counter = 0;

	ImGui::Begin("Scene");

	ID3D11ShaderResourceView* my_texture = Renderer::GetInstance()->GetBlendTexture();
	ImGui::Image((void*)my_texture, ImVec2(SCREENSIZEX, SCREENSIZEY));
	ImVec2 MinPos;

	Vector2 MousePos = Vector2();

	MinPos = ImGui::GetItemRectMin();
	ImVec2 sceneSize = ImGui::GetWindowSize();
	screenSize = Vector2(sceneSize.x, sceneSize.y);

	POINT p;
	GetCursorPos(&p);
	ScreenToClient(GhWnd, &p);

	MousePos = Vector2(ImGui::GetMousePos().x - MinPos.x, ImGui::GetMousePos().y - MinPos.y);

	//cout << MousePos.x << " , " << MousePos.y << endl;

	auto tempTerrain = Manage->FindGameObjectTest(STATIC, L"Terrain", L"Terrain");
	auto terrain = dynamic_pointer_cast<Terrain>(tempTerrain);
	//terrain->SetScreenSize(Vector2(sceneSize.x, sceneSize.y));
	//terrain->SetScreenPos(MousePos);
	terrain->PickTerrain(MousePos, screenSize);



	ImGui::End();
}

void ImguiManager::Menu1()
{
	ImGui::Begin("Menu1");



	ImGui::End();

}

void ImguiManager::Hierarchy()
{
	ImGui::Begin("Hierarchy");

	static bool popUp = false;


	if (ImGui::TreeNode("Scene"))
	{


		// For Popup.
		ObjectPopup();

		auto objects = Manage->GetObjectsForImgui();

		for (auto iter = objects.begin(); iter != objects.end(); ++iter)
		{
			
			if (ImGui::TreeNode(ToString(iter->first).c_str()))
			{
				isObjectInspector = true;
				objectForInspector = iter->second;
				ObjectPopup();
				ImGui::TreePop();
			}
			//if (ImGui::Selectable(ToString(iter->first).c_str()))
			//{
			//	isObjectInspector = true;
			//	objectForInspector = iter->second;
			//	break;
			//}

		}

		ImGui::TreePop();
	}



	ImGui::End();
}

void ImguiManager::Inspector()
{
	ImGui::Begin("Inspector");

	ObjectInspector();

	ImGui::End();
}

void ImguiManager::ObjectPopup(bool _isRoot)
{
	if (ImGui::IsItemClicked(ImGuiPopupFlags_MouseButtonRight))
		ImGui::OpenPopup("Object");

	if (ImGui::BeginPopupContextItem("Object"))
	{

		if (ImGui::MenuItem("Create Empty"))
			ImGui::CloseCurrentPopup();

		if (ImGui::BeginMenu("3D Object"))
		{
			ImGui::MenuItem("Cube");
			if (ImGui::MenuItem("Sphere"))
			{
				auto sphere = SphereTest::Create();
				Manage->AddLayerTest(SCENEID::STATIC, L"Sphere", sphere);

			}
			ImGui::MenuItem("Capsule");
			ImGui::MenuItem("Cylinder");


			ImGui::EndMenu();
		}
		if (ImGui::MenuItem("Effects"))
			ImGui::CloseCurrentPopup();

		if (ImGui::MenuItem("UI"))
		{
			auto ui = UI::Create();
			Manage->AddLayerTest(SCENEID::STATIC, L"UI", ui);
			ImGui::CloseCurrentPopup();
		}

		ImGui::EndPopup();
	}
}

void ImguiManager::ObjectInspector()
{
	if (isObjectInspector == false)
		return;

	auto obj = objectForInspector.lock();

	obj->RenderComponentsIspector();

	ImGui::Spacing();
	ImGui::Spacing();
	ImGui::SetCursorPosX((float)ImGui::GetWindowSize().x / 4);


	ComponentPopup();

}

void ImguiManager::ComponentPopup()
{
	if (ImGui::Button("Add Component", ImVec2((float)ImGui::GetWindowSize().x / 2.f, 30.f)))
		ImGui::OpenPopup("ComponentPopup");

	if (ImGui::BeginPopupContextItem("ComponentPopup"))
	{
		if (ImGui::BeginMenu("Physics"))
		{
			auto obj = objectForInspector.lock();

			if (ImGui::MenuItem("Rigidbody"))
				obj->AddComponent(ComponentType::RIGIDBODY, Rigidbody::Create(obj));

			if (ImGui::MenuItem("Sphere Collider"))
				obj->AddComponent(ComponentType::COLLIDER, Collider::Create(obj, PxGeometryType::eSPHERE));

			if (ImGui::MenuItem("Box Collider"))
				obj->AddComponent(ComponentType::COLLIDER, Collider::Create(obj, PxGeometryType::eBOX));

			if (ImGui::MenuItem("Capsule Collider"))
				obj->AddComponent(ComponentType::COLLIDER, Collider::Create(obj, PxGeometryType::eCAPSULE));



			ImGui::EndMenu();
		}
		ImGui::EndPopup();
	}

}

void ImguiManager::Frame()
{
	ImGui::Begin("Frame");

	RECT rc;
	GetClientRect(GhWnd, &rc);

	ImGui::End();
}
