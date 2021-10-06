#include "pch.h"
#include "Texture.h"


HRESULT Texture::Initialize(ID3D11Device * _Device, TEXTUREDESC* _decs)
{
	Decs = *_decs;

	ScratchImage image = LoadTextureFromFile();

	if (FAILED(CreateShaderResourceView(_Device, image.GetImages(), image.GetImageCount(), image.GetMetadata(), &texture)))
	{
		MSG_BOX("Failed to create Texture.");
		return E_FAIL;
	}
	return S_OK;
}

ScratchImage Texture::LoadTextureFromFile()
{
	// Load the texture.
	WCHAR ext[_MAX_EXT];
	wstring Path_W;
	Path_W.assign(Decs.Path.begin(), Decs.Path.end());

	_wsplitpath_s(Path_W.c_str(), nullptr, 0, nullptr, 0, nullptr, 0, ext, _MAX_EXT);

	ScratchImage image;

	if (_wcsicmp(ext, L".dds") == 0)
		LoadFromDDSFile(Path_W.c_str(), DDS_FLAGS_NONE, nullptr, image);

	else if (_wcsicmp(ext, L".tga") == 0)
		LoadFromTGAFile(Path_W.c_str(), nullptr, image);
	
	else if (_wcsicmp(ext, L".wic") == 0)
		LoadFromWICFile(Path_W.c_str(), WIC_FLAGS_NONE, nullptr, image);
	

	return image;
}

TEXTUREDESC & Texture::GetTextureDecs()
{
	return Decs;
}

shared_ptr <Texture> Texture::Create(ID3D11Device * _Device, TEXTUREDESC* _decs)
{
	shared_ptr<Texture> Instance = make_shared<Texture>();

	if (nullptr == Instance || FAILED(Instance->Initialize(_Device, _decs)))
	{
		MSG_BOX("Failed to create Texture.");
		return nullptr;
	}

	return Instance;
}

ID3D11ShaderResourceView ** Texture::GetTexture()
{
	return texture.GetAddressOf();
}
