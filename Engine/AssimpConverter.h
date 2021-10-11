#pragma once

#include "Includes.h"

class AssimpConverter
{
public:
	AssimpConverter();
	AssimpConverter(const AssimpConverter& Rhs);
	~AssimpConverter();

public:
	void ReadFile(wstring _File);
	void ExportMesh(wstring _SavePath);

	void ReadBoneData(aiNode* _Node, int _Index, int _Parent);
	void ReadMeshData(aiNode* _Node, int _Bone);
	void WriteMeshData(wstring _SavePath);

private:
	wstring File;

	Assimp::Importer* importer = nullptr;
	const aiScene* scene = nullptr;

	vector<asBone*> Bones;
	vector<asMesh*> Meshes;
};

// Assimp 구조
/*
Importer -> Scene (모델 정보가 있음)

Scene -> Materials(색상과 재질)

Scene -> Meshes (실제 렌더링될 데이터)

Scene -> Scene Root (부모, 자식 관계로 된 트렌스폼 정보를 가지고 있음)
         EX) Body
		       ㄴ바퀴 ---> Transformation(부모와 자식간의 연관간계를 유지 하기위해 들어가 있음), Material(Scene에 있는 Materials를 구분 할 번호), Mesh(Scene에 있는 Meshes를 구분 할 번호)
			   ㄴ바퀴
			   ㄴ포탑 
			       ㄴ 포신

*/