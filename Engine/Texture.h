#pragma once

#include "Includes.h"


class Texture
{
public:
	Texture() {}
	Texture(const Texture& Rhs) {}
	~Texture() {}

public:
	ID3D11ShaderResourceView**			GetTexture();
	TEXTUREDESC&						GetTextureDecs();


private:
	HRESULT								Initialize(ID3D11Device* _Device, TEXTUREDESC* _decs = nullptr);
	ScratchImage						LoadTextureFromFile();

public:
	static shared_ptr <Texture>			Create(ID3D11Device* _Device, TEXTUREDESC* _decs = nullptr);

private:
	ComPtr<ID3D11ShaderResourceView>	texture = nullptr;
	TEXTUREDESC							Decs;
};

