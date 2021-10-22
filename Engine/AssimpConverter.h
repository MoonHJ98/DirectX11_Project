#pragma once

#include "Includes.h"

class AssimpConverter
{
public:
	AssimpConverter();
	AssimpConverter(const AssimpConverter& Rhs);
	~AssimpConverter();

public:
	void				ReadFile(wstring _File);
	void				ExportMesh(wstring _SavePath);
	void				ExportMaterial(wstring _SavePath);
	void				ExportAnimClip(UINT index, wstring savePath);
	

private:
	void				ReadBoneData(aiNode* _Node, int _Index, int _Parent);
	void				ReadMeshData(aiNode* _Node, int _Bone);
	void				ReadSkinData();
	void				WriteMeshData(wstring _SavePath);
	void				ReadMaterialData();
	void				WriteMaterialData(wstring _SavePath);
	string				WriteTexture(string _SaveFolder, string _file);

public:
	void				ClipList(vector<wstring>* list);

private:
	asClip*				ReadClipData(aiAnimation* animation);
	void				ReadKeyframeData(asClip* clip, aiNode* node, vector<asClipNode>& aiNodeInfos);  // Ŭ���� � ������ �𸣱� ������ ReadClipData���� ���� Ŭ���� �״�� �Ѱ��༭ ó��
	void				WriteClipData(asClip* clip, wstring savePath);

private:
	wstring				File;

	Assimp::Importer*	importer = nullptr;
	const aiScene*		scene = nullptr;

	vector<asBone*>		Bones;
	vector<asMesh*>		Meshes;
	vector<asMaterial*> Materials;

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

/* �ִϸ��̼�
�޽��� ���� ���� ���� �׸� ���̰�  �ִϸ��̼��� ���� �� BoneŬ������ ��Ī ���� ����.
�ִϸ��̼ǿ� ���� �� �����Ӹ��� �������� �� Matrix�� ������ ����
Ű ������ �ִϸ��̼��̶���
�ǹ������� ���� ���ϴ� ������ Ű�����Ӹ� ���� �ϰ� �� ���̸� ���� ������. ���⼭�� ��� ������ ����.


*/