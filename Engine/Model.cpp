#include "pch.h"
#include "Model.h"
#include "AssimpConverter.h"


Model::Model()
{
	a = new AssimpConverter();

	a->ReadFile(L"Player/SK_CHR_Jack.FBX");
	a->ExportMesh(L"Player/SK_CHR_Jack.FBX");

}

Model::~Model()
{
}
