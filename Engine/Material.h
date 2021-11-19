#pragma once

#include "Component.h"

class Texture;
class GraphicDevice;

class Material : public Component
{
private:
	Material();
	Material(const Material& Rhs);

public:
	~Material();

public:
	wstring GetName()					 { return name; }
	Color& GetAmbient()					 { return colorDesc.Ambient; }
	Color& GetDiffuse()					 { return colorDesc.Diffuse; }
	Color& GetSpecular()				 { return colorDesc.Specular; }
	Color& GetEmissive()				 { return colorDesc.Emissive; }
	shared_ptr<Texture> GetDiffuseMap()  { return diffuseMap; }
	shared_ptr<Texture> GetSpecularMap() { return specularMap; }
	shared_ptr<Texture> GetNormalMap()   { return normalMap; }

private:
	void SetDiffuseMap(wstring file);
	void SetSpecularMap(wstring file);
	void SetNormalMap(wstring file);

public:
	virtual int Update(float _timeDelta) override;
	virtual void Render() override;

private:
	HRESULT Initialize(MATERIALDESC& desc);

private:
	struct ColorDesc
	{
		Color Ambient = Color(0, 0, 0, 1);
		Color Diffuse = Color(1, 1, 1, 1);
		Color Specular = Color(0, 0, 0, 1);
		Color Emissive = Color(0, 0, 0, 1);
	};


public:
	static shared_ptr<Material> Create(MATERIALDESC& desc);

private:
	shared_ptr<GraphicDevice> Graphic = nullptr;
	wstring name = L"";
	ColorDesc colorDesc = {};
	shared_ptr<Texture> diffuseMap = nullptr;
	shared_ptr<Texture> specularMap = nullptr;
	shared_ptr<Texture> normalMap = nullptr;

};