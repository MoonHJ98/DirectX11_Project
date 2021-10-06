#pragma once

#include "Includes.h"

class Texture;
class AnimationController;

class AssimpLoader
{
public:
	// static Ű����� ���� �ϳ��� ��� �Լ�&������ ����Ų��.
	static shared_ptr<AssimpLoader> GetInstance()
	{
		static shared_ptr<AssimpLoader> instance{ new AssimpLoader };
		return instance;
	}
private:
	AssimpLoader()
	{
		// ��ü ������ �ʱ�ȭ�� �ʿ��ϸ� ���⼭ �����Ѵ�.
	}

	AssimpLoader(const AssimpLoader& Rhs) = delete;

	AssimpLoader& operator=(const AssimpLoader& Rhs) = delete;

public:
	~AssimpLoader();

private:
	HRESULT				ReadFile(string _MeshFilePath, vector<MESH*>& _meshes, vector<shared_ptr<Texture>>& _textures, AnimationController** _Animator);
	void				ProcessNode(aiNode* _Node, vector<MESH*>& _meshes, vector<shared_ptr<Texture>>& _textures, AnimationController** _Animator);
	void				ProcessMesh(aiNode* node, vector<MESH*>& _meshes, vector<shared_ptr<Texture>>& _textures, AnimationController** _Animator);
	vector<Weights>		CalculateWeights(aiMesh * _mesh, AnimationController** _Animator);
	HRESULT				ProcessMaterial(MESH* _mesh, vector<shared_ptr<Texture>>& _textures);
	shared_ptr<Texture> MatchTexture(TEXTUREDESC& _texture, vector<shared_ptr<Texture>>& _textures);



public:
	HRESULT				LoadModel(string _MeshFilePath, vector<MESH*>& _meshes, vector<shared_ptr<Texture>>& _textures, AnimationController** _Animator);

private:
	Assimp::Importer	importer;
	const aiScene*		scene;
	string				FolderPath;
};

