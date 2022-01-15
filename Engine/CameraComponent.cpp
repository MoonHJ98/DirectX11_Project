#include "pch.h"
#include "CameraComponent.h"
#include "Camera.h"
#include "CamSphere.h"
#include "Line.h"
#include "Camera.h"

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
	if (rander)
	{
		RenderSpheres();
		RenderLine();
	}


}

void CameraComponent::RenderInspector()
{
	if (ImGui::CollapsingHeader("Camera"))
	{
		rander = true;

		CamList();

		EyeVectors();

		AtVectors();

		PlayCamera();
	}
	else
		rander = false;
}

void CameraComponent::GetEyeAtVector(vector<Vector3>& _eye, vector<Vector3>& _at)
{
	_eye.clear();
	_at.clear();
	auto EyeAt = EyeAtList.begin();
	for (int i = 0; i < camCurrentIndex; ++i)
		++EyeAt;

	if (!EyeAt->second.first.empty())
		_eye.assign(EyeAt->second.first.begin(), EyeAt->second.first.end());


	if (!EyeAt->second.second.empty())
		_at.assign(EyeAt->second.second.begin(), EyeAt->second.second.end());
}

HRESULT CameraComponent::Initialize(shared_ptr<GameObject> _object)
{
	object = _object;

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
		
		eyeCurrentIndex = 0;
		camCurrentIndex = camList.size() - 1;

		camSpheres.emplace(str, pair<vector<shared_ptr<CamSphere>>, vector<shared_ptr<CamSphere>>>());

		lineList.emplace(str, pair<shared_ptr<Line>, shared_ptr<Line>>());
		lineList[str].first = Line::Create();
		lineList[str].second = Line::Create();

		EyeAtList.emplace(str, pair<vector<Vector3>, vector<Vector3>>());
		str[0] = {};
	}
}

void CameraComponent::EyeVectors()
{
	ImGui::Spacing();


	ImGui::Text("Eye List");
	ImGui::Spacing();

	if (camList.empty())
		return;

	auto cam = camList.begin();
	auto camSphere = camSpheres.begin();
	auto EyeAt = EyeAtList.begin();
	auto line = lineList.begin();


	if (ImGui::BeginListBox("", ImVec2(ImGui::GetItemRectSize().x * 2.f, 5.f * ImGui::GetTextLineHeightWithSpacing())))
	{
		for (int i = 0; i < camCurrentIndex; ++i)
		{
			++cam;
			++camSphere;
			++EyeAt;
			++line;
		}

		for (size_t i = 0; i < cam->second.first.size(); ++i)
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

	if (ImGui::Button("EyeAdd"))
	{
		Vector3 pos = object.lock().get()->GetPosition();
		cam->second.first.push_back(pos);

		UpdateCamPos(&cam->second.first, nullptr);
		UpdateCamSphere(&camSphere->second.first, nullptr, true, Vector3(1.f, 1.f, 0.f));
		UpdateLine(&line->second.first, nullptr, EyeAt->second.first, Vector4(1.f, 0.f, 0.f, 1.f));
		eyeCurrentIndex = cam->second.first.size() - 1;
	}
	ImGui::SameLine();

	if (ImGui::Button("EyeDelete"))
	{
		auto _cam = cam->second.first.begin();
		for (int i = 0; i < eyeCurrentIndex; ++i)
		{
			++_cam;
		}
		cam->second.first.erase(_cam);
		UpdateCamPos(&cam->second.first, nullptr);
		UpdateCamSphere(&camSphere->second.first, nullptr, false);
		UpdateLine(&line->second.first, nullptr, EyeAt->second.first, Vector4(1.f, 0.f, 0.f, 1.f));
		eyeCurrentIndex = cam->second.first.size() - 1;
	}

	if (cam->second.first.empty())
	{
		float eyepos[3] = { 0.f, 0.f, 0.f };
		ImGui::InputFloat3("EyePos", eyepos);
		return;
	}

	float eyepos[3] = { cam->second.first[eyeCurrentIndex].x, cam->second.first[eyeCurrentIndex].y, cam->second.first[eyeCurrentIndex].z };
	ImGui::InputFloat3("EyePos", eyepos);
}

void CameraComponent::AtVectors()
{
	ImGui::Spacing();


	ImGui::Text("At List");
	ImGui::Spacing();

	if (camList.empty())
		return;

	auto cam = camList.begin();
	auto camSphere = camSpheres.begin();
	auto EyeAt = EyeAtList.begin();
	auto line = lineList.begin();


	if (ImGui::BeginListBox(" ", ImVec2(ImGui::GetItemRectSize().x * 2.f, 5.f * ImGui::GetTextLineHeightWithSpacing())))
	{
		for (int i = 0; i < camCurrentIndex; ++i)
		{
			++cam;
			++camSphere;
			++EyeAt;
			++line;
		}

		for (size_t i = 0; i < cam->second.second.size(); ++i)
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

	if (ImGui::Button("AtAdd"))
	{
		Vector3 pos = object.lock().get()->GetPosition();
		cam->second.second.push_back(pos);

		UpdateCamPos(nullptr, &cam->second.second);
		UpdateCamSphere(nullptr, &camSphere->second.second, true, Vector3(0.f, 1.f, 1.f));
		UpdateLine(nullptr, &line->second.second, EyeAt->second.second, Vector4(1.f, 1.f, 1.f, 1.f));
		atCurrentIndex = cam->second.second.size() - 1;
	}
	ImGui::SameLine();

	if (ImGui::Button("AtDelete"))
	{
		auto _cam = cam->second.second.begin();
		for (int i = 0; i < atCurrentIndex; ++i)
		{
			++_cam;
		}
		cam->second.second.erase(_cam);
		UpdateCamPos(nullptr, &cam->second.second);
		UpdateCamSphere(nullptr, &camSphere->second.second, false);
		UpdateLine(nullptr, &line->second.second, EyeAt->second.second, Vector4(1.f, 0.f, 0.f, 1.f));
		atCurrentIndex = cam->second.second.size() - 1;
	}

	if (cam->second.second.empty())
	{
		float atpos[3] = { 0.f, 0.f, 0.f };
		ImGui::InputFloat3("AtPos", atpos);
		return;
	}

	float atpos[3] = { cam->second.second[atCurrentIndex].x, cam->second.second[atCurrentIndex].y, cam->second.second[atCurrentIndex].z };
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
	for (UINT i = 0; i < iter->second.second.size(); ++i)
	{
		iter->second.second[i]->Render();
	}
}

void CameraComponent::RenderLine()
{
	if (lineList.empty())
		return;

	auto iter = lineList.begin();

	for (int i = 0; i < camCurrentIndex; ++i)
		++iter;

	iter->second.first->Render();
	iter->second.second->Render();


}

void CameraComponent::UpdateCamPos(vector<Vector3>* _eyePos, vector<Vector3>* _atPos)
{
	if (_eyePos == nullptr && _atPos == nullptr)
		return;

	auto EyeAt = EyeAtList.begin();

	for (int i = 0; i < camCurrentIndex; ++i)
	{
		++EyeAt;
	}

	if (_eyePos != nullptr)
	{
		EyeAt->second.first.clear();

		if (_eyePos->size() < 2)
		{
			for (UINT i = 0; i < _eyePos->size(); ++i)
				EyeAt->second.first.push_back((*_eyePos)[i]);

		}
		else if (_eyePos->size() == 2)
		{
			for (size_t i = 0; i < _eyePos->size() - 1; ++i)
			{
				for (int j = 0; j < 100; ++j)
				{
					Vector3 result;

					float rate_cat_mull = (j + 1) / 100.f;

					result = XMVectorCatmullRom(XMLoadFloat3(&(*_eyePos)[0]), XMLoadFloat3(&(*_eyePos)[0]), XMLoadFloat3(&(*_eyePos)[1]), XMLoadFloat3(&(*_eyePos)[1]), rate_cat_mull);

					EyeAt->second.first.push_back(result);
				}
			}
		}
		else
		{

			for (UINT i = 0; i < _eyePos->size() - 1; ++i)
			{
				for (int j = 0; j < 100; ++j)
				{
					Vector3 result;

					float rate_cat_mull = (j + 1) / 100.f;


					if (i == 0)
						result = XMVectorCatmullRom(XMLoadFloat3(&(*_eyePos)[i]), XMLoadFloat3(&(*_eyePos)[i]), XMLoadFloat3(&(*_eyePos)[i + 1]), XMLoadFloat3(&(*_eyePos)[i + 2]), rate_cat_mull);
					else if (i == _eyePos->size() - 2)
						result = XMVectorCatmullRom(XMLoadFloat3(&(*_eyePos)[i - 1]), XMLoadFloat3(&(*_eyePos)[i]), XMLoadFloat3(&(*_eyePos)[i + 1]), XMLoadFloat3(&(*_eyePos)[i + 1]), rate_cat_mull);
					else
						result = XMVectorCatmullRom(XMLoadFloat3(&(*_eyePos)[i - 1]), XMLoadFloat3(&(*_eyePos)[i]), XMLoadFloat3(&(*_eyePos)[i + 1]), XMLoadFloat3(&(*_eyePos)[i + 2]), rate_cat_mull);

					EyeAt->second.first.push_back(result);

				}
			}

		}
	}

	if (_atPos != nullptr)
	{
		EyeAt->second.second.clear();

		if (_atPos->size() < 2)
		{
			for (UINT i = 0; i < _atPos->size(); ++i)
				EyeAt->second.second.push_back((*_atPos)[i]);

		}
		else if (_atPos->size() == 2)
		{
			for (size_t i = 0; i < _atPos->size() - 1; ++i)
			{
				for (int j = 0; j < 100; ++j)
				{
					Vector3 result;

					float rate_cat_mull = (j + 1) / 100.f;

					result = XMVectorCatmullRom(XMLoadFloat3(&(*_atPos)[0]), XMLoadFloat3(&(*_atPos)[0]), XMLoadFloat3(&(*_atPos)[1]), XMLoadFloat3(&(*_atPos)[1]), rate_cat_mull);

					EyeAt->second.second.push_back(result);
				}
			}
		}
		else
		{

			for (UINT i = 0; i < _atPos->size() - 1; ++i)
			{
				for (int j = 0; j < 100; ++j)
				{
					Vector3 result;

					float rate_cat_mull = (j + 1) / 100.f;


					if (i == 0)
						result = XMVectorCatmullRom(XMLoadFloat3(&(*_atPos)[i]), XMLoadFloat3(&(*_atPos)[i]), XMLoadFloat3(&(*_atPos)[i + 1]), XMLoadFloat3(&(*_atPos)[i + 2]), rate_cat_mull);
					else if (i == _atPos->size() - 2)
						result = XMVectorCatmullRom(XMLoadFloat3(&(*_atPos)[i - 1]), XMLoadFloat3(&(*_atPos)[i]), XMLoadFloat3(&(*_atPos)[i + 1]), XMLoadFloat3(&(*_atPos)[i + 1]), rate_cat_mull);
					else
						result = XMVectorCatmullRom(XMLoadFloat3(&(*_atPos)[i - 1]), XMLoadFloat3(&(*_atPos)[i]), XMLoadFloat3(&(*_atPos)[i + 1]), XMLoadFloat3(&(*_atPos)[i + 2]), rate_cat_mull);

					EyeAt->second.second.push_back(result);

				}
			}

		}
	}

}

void CameraComponent::UpdateLine(shared_ptr<Line>* _eyeLine, shared_ptr<Line>* _atLine, vector<Vector3>& _pos, Vector4 _color)
{
	if (_eyeLine == nullptr && _atLine == nullptr)
		return;

	if(_eyeLine)
		(*_eyeLine).get()->UpdateBuffer(_pos, _color);
	if (_atLine)
		(*_atLine).get()->UpdateBuffer(_pos, _color);

}

void CameraComponent::UpdateCamSphere(vector<shared_ptr<CamSphere>>* _eyeSphere, vector<shared_ptr<CamSphere>>* _atSphere, bool _addSphere, Vector3 _color)
{
	if (_eyeSphere == nullptr && _atSphere == nullptr)
		return;

	if (_addSphere)
	{
		if (_eyeSphere)
		{
			Vector3 pos = object.lock().get()->GetPosition();
			_eyeSphere->push_back(CamSphere::Create(pos, _color));
		}
		else if(_atSphere)
		{
			Vector3 pos = object.lock().get()->GetPosition();
			_atSphere->push_back(CamSphere::Create(pos, _color));
		}
	}
	else
	{
		if (_eyeSphere)
		{
			auto currentSphere = _eyeSphere->begin();
			for (int i = 0; i < eyeCurrentIndex; ++i)
				++currentSphere;
			
			_eyeSphere->erase(currentSphere);

			eyeCurrentIndex = _eyeSphere->size() - 1;
		}
		else if (_atSphere)
		{
			auto currentSphere = _atSphere->begin();
			for (int i = 0; i < atCurrentIndex; ++i)
				++currentSphere;

			_atSphere->erase(currentSphere);

			atCurrentIndex = _atSphere->size() - 1;
		}
	}
}

void CameraComponent::PlayCamera()
{
	ImGui::Spacing();

	if (ImGui::Button("Play", ImVec2((float)ImGui::GetWindowSize().x / 3.f, 0.f)))
	{
		playButtonPressed = true;
	}
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
