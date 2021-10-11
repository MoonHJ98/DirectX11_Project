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

// Assimp ����
/*
Importer -> Scene (�� ������ ����)

Scene -> Materials(����� ����)

Scene -> Meshes (���� �������� ������)

Scene -> Scene Root (�θ�, �ڽ� ����� �� Ʈ������ ������ ������ ����)
         EX) Body
		       ������ ---> Transformation(�θ�� �ڽİ��� �������踦 ���� �ϱ����� �� ����), Material(Scene�� �ִ� Materials�� ���� �� ��ȣ), Mesh(Scene�� �ִ� Meshes�� ���� �� ��ȣ)
			   ������
			   ����ž 
			       �� ����

*/