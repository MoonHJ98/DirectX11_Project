#include "pch.h"
#include "CameraComponent.h"
#include "Camera.h"
#include "CamSphere.h"
#include "Line.h"

CameraComponent::CameraComponent()
{
}

CameraComponent::CameraComponent(const CameraComponent & Rhs)
{
}

CameraComponent::~CameraComponent()
{
}

int CameraComponent::Update(float _timeDelta)
{
	return 0;
}

void CameraComponent::Render()
{
	if(randerSpheres)
		RenderSpheres();
}

void CameraComponent::RenderInspector()
{
	if (ImGui::CollapsingHeader("Camera"))
	{
		randerSpheres = true;

		CamList();

		EyeVectors();

		AtVectors();
	}
	else
		randerSpheres = false;
}

HRESULT CameraComponent::Initialize(shared_ptr<GameObject> _object)
{
	object = _object;
	line = Line::Create();

	return S_OK;
}

void CameraComponent::CamList()
{
	ImGui::Spacing();

	
	auto iter = camList.begin();

	for (int i = 0; i < camCurrentIndex; ++i)
		++iter;

	if (!camList.empty())
	{
		const char* combo_preview_value = iter->first.c_str();

		if (ImGui::BeginCombo("Cam List", combo_preview_value))
		{
			int index = 0;
			for (auto iter = camList.begin(); iter != camList.end(); ++iter)
			{
				const bool is_selected = (camCurrentIndex == index);
				if (ImGui::Selectable(iter->first.c_str(), is_selected))
				{
					camCurrentIndex = index;
					eyeCurrentIndex = 0;
				}


				if (is_selected)
					ImGui::SetItemDefaultFocus();

				++index;
			}
			ImGui::EndCombo();
		}
	}

	AddCam();
}

void CameraComponent::AddCam()
{
	static char str[256] = "";
	ImGui::InputTextWithHint(" ", "enter text here", str, IM_ARRAYSIZE(str));

	

	if (ImGui::Button("Add Cam"))
	{
		if (strlen(str) == 0)
			return;

		camList.emplace(str, pair<vector<Vector3>, vector<Vector3>>());
		str[0] = {};
		eyeCurrentIndex = 0;
		camCurrentIndex = camList.size() - 1;

		camSpheres.emplace(str, pair<vector<shared_ptr<CamSphere>>, vector<shared_ptr<CamSphere>>>());
	}
}

void CameraComponent::EyeVectors()
{
	ImGui::Spacing();


	ImGui::Text("Eye List");
	ImGui::Spacing();

	if (camList.empty())
		return;

	auto eye = camList.begin();
	auto camSphere = camSpheres.begin();

	
	if (ImGui::BeginListBox("", ImVec2(ImGui::GetItemRectSize().x * 2.f, 5.f * ImGui::GetTextLineHeightWithSpacing())))
	{
		for (int i = 0; i < camCurrentIndex; ++i)
		{
			++eye;
			++camSphere;
		}
		
		for (size_t i = 0; i < eye->second.first.size(); ++i)
		{
			const bool is_selected = (eyeCurrentIndex == i);

			string eyeStr = "Eye " + to_string(i);
			if (ImGui::Selectable(eyeStr.c_str(), is_selected))
				eyeCurrentIndex = i;

			if (is_selected)
				ImGui::SetItemDefaultFocus();


		}
		ImGui::EndListBox();
	}

	ImGui::SameLine();

	if (ImGui::Button("Add"))
	{
		Vector3 pos = object.lock().get()->GetPosition();
		eye->second.first.push_back(pos);
		camSphere->second.first.push_back(CamSphere::Create(pos, Vector3(1.f, 1.f, 0.f)));
		line->UpdateBuffer(eye->second.first, Vector4(1.f, 0.f, 0.f, 1.f));

	}
	ImGui::SameLine();
	ImGui::Button("Delete");

	if (eye->second.first.empty())
	{
		float eyepos[3] = { 0.f, 0.f, 0.f };
		ImGui::InputFloat3("EyePos", eyepos);
		return;
	}
	float eyepos[3] = { eye->second.first[eyeCurrentIndex].x, eye->second.first[eyeCurrentIndex].y, eye->second.first[eyeCurrentIndex].z };
	ImGui::InputFloat3("EyePos", eyepos);
}

void CameraComponent::AtVectors()
{
	ImGui::Spacing();


	ImGui::Text("At List");
	ImGui::Spacing();

	if (camList.empty())
		return;

	auto at = camList.begin();


	if (ImGui::BeginListBox(" ", ImVec2(ImGui::GetItemRectSize().x * 2.f, 5.f * ImGui::GetTextLineHeightWithSpacing())))
	{
		for (int i = 0; i < camCurrentIndex; ++i)
		{
			++at;
		}

		for (size_t i = 0; i < at->second.second.size(); ++i)
		{
			const bool is_selected = (atCurrentIndex == i);

			string atStr = "At " + to_string(i);
			if (ImGui::Selectable(atStr.c_str(), is_selected))
				atCurrentIndex = i;

			if (is_selected)
				ImGui::SetItemDefaultFocus();


		}
		ImGui::EndListBox();
	}

	ImGui::SameLine();

	if (ImGui::Button("Add"))
	{
		//Vector3 pos = object.lock().get()->GetPosition();
		//at->second.first.push_back(pos);
	}
	ImGui::SameLine();
	ImGui::Button("Delete");

	if (at->second.first.empty())
	{
		float atpos[3] = { 0.f, 0.f, 0.f };
		ImGui::InputFloat3("AtPos", atpos);
		return;
	}
	float atpos[3] = { at->second.first[atCurrentIndex].x, at->second.first[atCurrentIndex].y, at->second.first[atCurrentIndex].z };
	ImGui::InputFloat3("AtPos", atpos);
}

void CameraComponent::RenderSpheres()
{
	if (camSpheres.empty())
		return;

	auto iter = camSpheres.begin();

	for (int i = 0; i < camCurrentIndex; ++i)
		++iter;

	for (UINT i = 0; i < iter->second.first.size(); ++i)
	{
		iter->second.first[i]->Render();
	}

	line->Render();
}

shared_ptr<CameraComponent> CameraComponent::Create(shared_ptr<GameObject> _object)
{
	shared_ptr<CameraComponent> Instance(new CameraComponent());
	if (FAILED(Instance->Initialize(_object)))
	{
		MSG_BOX("Failed to create CameraComponent.");
		return nullptr;
	}
	return Instance;
}
