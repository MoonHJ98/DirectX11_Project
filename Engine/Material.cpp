#include "pch.h"
#include "Material.h"
#include "Texture.h"
#include "GraphicDevice.h"

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
	Graphic = GraphicDevice::GetInstance();

	name = desc.name;

	colorDesc.Ambient = desc.Ambient;
	colorDesc.Diffuse = desc.Diffuse;
	colorDesc.Emissive = desc.Emissive;
	colorDesc.Specular = desc.Specular;

	SetDiffuseMap(desc.DiffuseMap);
	SetNormalMap(desc.NormalMap);
	SetSpecularMap(desc.SpecularMap);

	return S_OK;
}

void Material::SetDiffuseMap(wstring file)
{
	diffuseMap.reset();

	if (file == L"")
		return;

	TEXTUREDESC desc;
	desc.MaterialName = name;
	desc.Path = file;
	desc.Type = L"Diffuse";

	diffuseMap = Texture::Create(Graphic->GetDevice(), &desc);
}


void Material::SetSpecularMap(wstring file)
{
	specularMap.reset();

	if (file == L"")
		return;

	TEXTUREDESC desc;
	desc.MaterialName = name;
	desc.Path = file;
	desc.Type = L"Specular";

	specularMap = Texture::Create(Graphic->GetDevice(), &desc);
}


void Material::SetNormalMap(wstring file)
{
	normalMap.reset();

	if (file == L"")
		return;


	TEXTUREDESC desc;
	desc.MaterialName = name;
	desc.Path = file;
	desc.Type = L"Normal";

	normalMap = Texture::Create(Graphic->GetDevice(), &desc);
}

void Material::Render()
{
	if(diffuseMap)
		Graphic->GetDeviceContext()->PSSetShaderResources(0, 1, diffuseMap->GetTexture());

	if (normalMap)
		Graphic->GetDeviceContext()->PSSetShaderResources(1, 1, normalMap->GetTexture());

	if (specularMap)
		Graphic->GetDeviceContext()->PSSetShaderResources(2, 1, specularMap->GetTexture());

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
