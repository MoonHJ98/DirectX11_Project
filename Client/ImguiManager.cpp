#include "pch.h"
#include "ImguiManager.h"
#include "GraphicDevice.h"
#include "Renderer.h"
#include "Management.h"
#include "light.h"
#include "Terrain.h"

HRESULT ImguiManager::Initialize()
{
	Manage = Management::GetInstance();

	IMGUI_CHECKVERSION();
	ImGui::CreateContext();
	
	
	ImGui::GetIO().ConfigFlags |= ImGuiConfigFlags_DockingEnable;
	ImGui::GetIO().ConfigFlags |= ImGuiConfigFlags_ViewportsEnable;
	ImGui::GetIO().ConfigFlags |= ImGuiConfigFlags_NavEnableSetMousePos;





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

	DirectionalLight();


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

void ImguiManager::DirectionalLight()
{
	static int counter = 0;

	ImGui::Begin("Scene");
	//ImGui::Text("Light Direction");
	//auto light = Manage->FindLight(L"DirectionalLight", 0);
	//Vector3 direction = light->GetLightInfo().Direction;
	//float dir[3]{ direction.x, direction.y, direction.z };
	//ImGui::DragFloat3("", dir, 0.1f, -1.f, 1.f, nullptr, 1);
	//light->SetLightDirection(Vector3(dir[0], dir[1], dir[2]));

	ID3D11ShaderResourceView* my_texture = Renderer::GetInstance()->GetBlendTexture();
	ImGui::Image((void*)my_texture, ImVec2(800.f, 600.f));

	ImVec2 MinPos;

	Vector2 MousePos;
	
	MinPos = ImGui::GetItemRectMin();


	MousePos = Vector2(ImGui::GetMousePos().x - MinPos.x, ImGui::GetMousePos().y - MinPos.y);
	
	// TODO : 이거 없애면 디퓨즈 빠지는거 해결하기.
	//auto tempTerrain = Manage->FindGameObject(STATIC, L"Terrain");
	//auto terrain = dynamic_pointer_cast<Terrain>(tempTerrain);
	//terrain->PickTerrain(MousePos);

	cout <<MousePos.x<< " , " <<MousePos.y<< endl;

	
	ImGui::End();
}
