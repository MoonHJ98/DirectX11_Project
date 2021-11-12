#include "pch.h"
#include "ImguiManager.h"
#include "GraphicDevice.h"

HRESULT ImguiManager::Initialize()
{
	IMGUI_CHECKVERSION();
	ImGui::CreateContext();
	ImGuiIO& io = ImGui::GetIO();
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

	static int counter = 0;
	// Create ImGui Test Window.
	ImGui::Begin("Test");
	ImGui::Text("This is example text.");
	if (ImGui::Button("Click me"))
		counter += 1;

	string clickCount = "Click Count : " + to_string(counter);
	ImGui::SameLine();
	ImGui::Text(clickCount.c_str());

	ImGui::End();

	// Assemble Together Draw Data
	ImGui::Render();
	// Render Draw Data
	ImGui_ImplDX11_RenderDrawData(ImGui::GetDrawData());
}
