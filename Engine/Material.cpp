#include "pch.h"
#include "Material.h"
#include "Texture.h"
#include "GraphicDevice.h"
#include "Path.h"

Material::Material()
{
}

Material::Material(const Material& Rhs)
{	
}

Material::~Material()
{

}

HRESULT Material::Initialize(MATERIALDESC& desc)
{
	componentType = MATERIAL;
	shared_ptr<ConstantBuffer<MaterialBufferDesc>> temp(new ConstantBuffer<MaterialBufferDesc>());
	materialBuffer = temp;


	Graphic = GraphicDevice::GetInstance();

	materialBuffer->Create(Graphic->GetDevice());

	name = desc.name;

	colorDesc.Ambient = desc.Ambient;
	colorDesc.Diffuse = desc.Diffuse;
	colorDesc.Emissive = desc.Emissive;
	colorDesc.Specular = desc.Specular;

	SetDiffuseMap(desc.DiffuseMap);
	SetNormalMap(desc.NormalMap);
	SetSpecularMap(desc.SpecularMap);
	NoImage();

	return S_OK;
}

void Material::RenderBrowser()
{
	if (ImGuiFileDialog::Instance()->Display("Browse"))
	{
		// action if OK
		if (ImGuiFileDialog::Instance()->IsOk())
		{
			string filePathName = ImGuiFileDialog::Instance()->GetFilePathName();
			string filePath = ImGuiFileDialog::Instance()->GetCurrentPath();

			// action
			switch (button)
			{
			case Material::DIFFUSE_BUTTON:
				SetDiffuseMap(ToWString(filePathName));
				break;
			case Material::NORMAL_BUTTON:
				SetNormalMap(ToWString(filePathName));
				break;
			case Material::SPECULAR_BUTTON:
				SetSpecularMap(ToWString(filePathName));
				break;
			case Material::BUTTON_END:
				break;
			default:
				break;
			}
		}

		// close
		ImGuiFileDialog::Instance()->Close();
	}
}

void Material::SetDiffuseMap(wstring file)
{
	diffuseMap.reset();

	if (file == L"")
		return;

	materialDesc.diffuse = TRUE;
	TEXTUREDESC desc;
	desc.MaterialName = name;
	desc.Path = file;
	desc.Type = L"Diffuse";
	diffuseName = Path::GetFileName(desc.Path);

	diffuseMap = Texture::Create(Graphic->GetDevice(), &desc);
}


void Material::SetSpecularMap(wstring file)
{
	specularMap.reset();

	if (file == L"")
		return;

	materialDesc.specular = TRUE;

	TEXTUREDESC desc;
	desc.MaterialName = name;
	desc.Path = file;
	desc.Type = L"Specular";
	specularName = Path::GetFileName(desc.Path);

	specularMap = Texture::Create(Graphic->GetDevice(), &desc);
}


void Material::SetNormalMap(wstring file)
{
	normalMap.reset();

	if (file == L"")
		return;

	materialDesc.normal = TRUE;

	TEXTUREDESC desc;
	desc.MaterialName = name;
	desc.Path = file;
	desc.Type = L"Normal";
	normalName = Path::GetFileName(desc.Path);

	normalMap = Texture::Create(Graphic->GetDevice(), &desc);
}

void Material::NoImage()
{
	TEXTUREDESC desc;
	desc.MaterialName = name;
	desc.Path = L"../Resources/test.tga";
	desc.Type = L"NoImage";
	

	noImage = Texture::Create(Graphic->GetDevice(), &desc);
}

void Material::Render()
{
	materialBuffer->SetData(Graphic->GetDeviceContext(), materialDesc);
	auto buffer = materialBuffer->GetBuffer();

	Graphic->GetDeviceContext()->PSSetConstantBuffers(1, 1, &buffer);

	if(diffuseMap)
		Graphic->GetDeviceContext()->PSSetShaderResources(0, 1, diffuseMap->GetTexture());

	if (normalMap)
		Graphic->GetDeviceContext()->PSSetShaderResources(1, 1, normalMap->GetTexture());

	if (specularMap)
		Graphic->GetDeviceContext()->PSSetShaderResources(2, 1, specularMap->GetTexture());

}

void Material::RenderInspector()
{
	if (ImGui::CollapsingHeader("Material"))
	{
		ImGui::Text("Diffuse");
		ImGui::Spacing();

		ID3D11ShaderResourceView* diffuse = nullptr;
		if (diffuseMap != nullptr)
			diffuse = *diffuseMap->GetTexture();
		else
			diffuse = *noImage->GetTexture();

		ImGui::Image((void*)diffuse, ImVec2(100.f, 100.f));
		ImGui::SameLine();

		ImGui::BeginGroup();

		if (diffuseName == L"")
			diffuseName = L"No Image";
		ImGui::Text(ToString(diffuseName).c_str());

		if (ImGui::Button("Browse##Diffuse"))
		{
			ImGuiFileDialog::Instance()->Close();
			ImGuiFileDialog::Instance()->OpenDialog("Browse", "Browser", ".*,.png,.tga,.mesh,.clip", "");
			button = DIFFUSE_BUTTON;
		}
	
		ImGui::EndGroup();

		ImGui::Spacing();
		ImGui::Spacing();


		ImGui::Text("Normal");
		ImGui::Spacing();

		ID3D11ShaderResourceView* normal = nullptr;
		if (normalMap != nullptr)
			normal = *normalMap->GetTexture();
		else
			normal = *noImage->GetTexture();

		ImGui::Image((void*)normal, ImVec2(100.f, 100.f));
		ImGui::SameLine();

		ImGui::BeginGroup();

		if (normalName == L"")
			normalName = L"No Image";
		ImGui::Text(ToString(normalName).c_str());
		if (ImGui::Button("Browse##Normal"))
		{
			ImGuiFileDialog::Instance()->Close();
			ImGuiFileDialog::Instance()->OpenDialog("Browse", "Browser", ".*,.png,.tga,.mesh,.clip", "");
			button = NORMAL_BUTTON;

		}

		ImGui::EndGroup();

		ImGui::Spacing();
		ImGui::Spacing();

		ImGui::Text("Specular");
		ImGui::Spacing();

		ID3D11ShaderResourceView* specular = nullptr;
		if (specularMap != nullptr)
			specular = *specularMap->GetTexture();
		else
			specular = *noImage->GetTexture();

		ImGui::Image((void*)specular, ImVec2(100.f, 100.f));
		ImGui::SameLine();

		ImGui::BeginGroup();

		if (specularName == L"")
			specularName = L"No Image";
		ImGui::Text(ToString(specularName).c_str());
		if (ImGui::Button("Browse##Specular"))
		{
			ImGuiFileDialog::Instance()->Close();
			ImGuiFileDialog::Instance()->OpenDialog("Browse", "Browser", ".*,.png,.tga,.mesh,.clip", "");
			button = SPECULAR_BUTTON;

		}

		ImGui::EndGroup();

		RenderBrowser();
	}

}

shared_ptr<Material> Material::Create(MATERIALDESC& desc)
{
	shared_ptr<Material> Instance(new Material());
	if (FAILED(Instance->Initialize(desc)))
	{
		MSG_BOX("Failed to create Material.");
		return S_OK;
	}
	return Instance;
}

int Material::Update(float _timeDelta)
{
	return 0;
}
