#include "pch.h"
#include "CameraComponent.h"

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
}

void CameraComponent::RenderInspector()
{
	if (ImGui::CollapsingHeader("Camera"))
	{
		static int eyeCurrentIndex = 0;

		// Eye
		{
			ImGui::Text("Eye List");
			ImGui::Spacing();
			if (ImGui::BeginListBox("", ImVec2(ImGui::GetItemRectSize().x * 2.f, 5.f * ImGui::GetTextLineHeightWithSpacing())))
			{
				for (size_t i = 0; i < eyePos.size(); ++i)
				{
					const bool is_selected = (eyeCurrentIndex == i);

					string eye = "Eye " + to_string(i);
					if (ImGui::Selectable(eye.c_str(), is_selected))
						eyeCurrentIndex = i;

					if (is_selected)
						ImGui::SetItemDefaultFocus();


				}
				ImGui::EndListBox();
			}

			ImGui::SameLine();
			ImGui::Button("Add");
			ImGui::SameLine();
			ImGui::Button("Delete");


			float eyepos[3] = { eyePos[eyeCurrentIndex].x, eyePos[eyeCurrentIndex].y, eyePos[eyeCurrentIndex].z };
			ImGui::InputFloat3("Pos", eyepos);

		}

		// At
		{

		}
	}
}

HRESULT CameraComponent::Initialize()
{
	eyePos.push_back(Vector3(0.f, 0.f, 0.f));
	eyePos.push_back(Vector3(0.f, 1.f, 0.f));
	eyePos.push_back(Vector3(0.f, 0.f, 1.f));

	return S_OK;
}

shared_ptr<CameraComponent> CameraComponent::Create()
{
	shared_ptr<CameraComponent> Instance(new CameraComponent());
	if (FAILED(Instance->Initialize()))
	{
		MSG_BOX("Failed to create CameraComponent.");
		return nullptr;
	}
	return Instance;
}
