#pragma once

#include "Includes.h"
#include <fbxsdk.h>


class FBXConverter
{
public:
	FBXConverter();
	FBXConverter(const FBXConverter& Rhs);
	~FBXConverter();

public:
	HRESULT				ReadFile(wstring _File);
	void				ExportMesh(wstring _SavePath);
	void				ReadBoneData(FbxNode* _Node, int _Index, int _Parent);



private:
	FbxManager* FbxSdkManager = nullptr;
	FbxScene* fbxScene = nullptr;
	wstring File;
};

