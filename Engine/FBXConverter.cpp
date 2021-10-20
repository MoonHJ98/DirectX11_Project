#include "pch.h"
#include "FBXConverter.h"

FBXConverter::FBXConverter()
{
}

FBXConverter::FBXConverter(const FBXConverter & Rhs)
{
}

FBXConverter::~FBXConverter()
{
}

HRESULT FBXConverter::ReadFile(wstring _File)
{
	this->File = L"../Resources/" + _File;

	if (FbxSdkManager == nullptr)
	{
		FbxSdkManager = FbxManager::Create();

		FbxIOSettings* pIOsettings = FbxIOSettings::Create(FbxSdkManager, IOSROOT);
		FbxSdkManager->SetIOSettings(pIOsettings);
	}

	FbxImporter* Importer = FbxImporter::Create(FbxSdkManager, "");
	fbxScene = FbxScene::Create(FbxSdkManager, "");

	if (false == Importer->Initialize(ToString(File).c_str(), -1, FbxSdkManager->GetIOSettings()))
	{
		MSG_BOX("Failed to read Fbx file.");
		return S_OK;
	}

	if (false == Importer->Import(fbxScene))
	{
		MSG_BOX("Failed to import fbxScene.");
		return E_FAIL;
	}

	Importer->Destroy();


	return S_OK;
}

void FBXConverter::ExportMesh(wstring _SavePath)
{
	_SavePath = L"../Resources/" + _SavePath + L".mesh";

	ReadBoneData(fbxScene->GetRootNode(), -1, -1);
}

void FBXConverter::ReadBoneData(FbxNode * _Node, int _Index, int _Parent)
{
	asBone* bone = new asBone();
	bone->Index = _Index;
	bone->Parent = _Parent;
	bone->Name = _Node->GetName();
	


}
