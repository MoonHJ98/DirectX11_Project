#include "pch.h"
#include "AssimpLoader.h"
#include <filesystem>
#include "GraphicDevice.h"
#include "Texture.h"
#include "AnimationController.h"

using namespace experimental;


AssimpLoader::~AssimpLoader()
{
}

HRESULT AssimpLoader::ReadFile(string _MeshFilePath, vector<shared_ptr<MESH>>& _meshes, vector<shared_ptr<Texture>>& _textures, shared_ptr<AnimationController>* _Animator)
{
	filesystem::path p = _MeshFilePath;
	p = p.parent_path();
	FolderPath = p.string();


	scene = importer.ReadFile
	(
		_MeshFilePath,
		aiProcess_ConvertToLeftHanded |		// 왼손좌표계
		aiProcess_Triangulate |				// 삼각형으로 바꾸기
		aiProcess_GenUVCoords |				// 모델 형식에 맞게 UV다시 계산
		aiProcess_GenNormals	            // Normal Vector 다시 계산
	);

	if (scene == nullptr)
	{
		MSG_BOX("Failed to load mesh file.");
		return E_FAIL;
	}

	
	*_Animator = AnimationController::Create(scene);


	ProcessNode(scene->mRootNode, _meshes, _textures, _Animator);

	return S_OK;
}

void AssimpLoader::ProcessNode(aiNode* _Node, vector<shared_ptr<MESH>>& _meshes, vector<shared_ptr<Texture>>& _textures, shared_ptr<AnimationController>* _Animator)
{
	ProcessMesh(_Node, _meshes, _textures, _Animator);

	for (UINT i = 0; i < _Node->mNumChildren; ++i)
		ProcessNode(_Node->mChildren[i], _meshes, _textures, _Animator);
}

void AssimpLoader::ProcessMesh(aiNode * node, vector<shared_ptr<MESH>>& _meshes, vector<shared_ptr<Texture>>& _textures, shared_ptr<AnimationController>* _Animator)
{
	if (node->mNumMeshes < 1)
		return;



	for (UINT i = 0; i < node->mNumMeshes; ++i)
	{
		shared_ptr<MESH> mesh(new MESH());
		mesh->Name = node->mName.C_Str();

		UINT index = node->mMeshes[i];
		aiMesh* _Mesh = scene->mMeshes[index];

		vector<WEIGHTS> weights = CalculateWeights(_Mesh, _Animator);


		aiMaterial* material = scene->mMaterials[_Mesh->mMaterialIndex];
		mesh->MaterialName = material->GetName().C_Str();

		vector<SkinnedMesh> vertices;
		vector<UINT> indices;

		UINT startVertex = vertices.size();

		for (UINT i = 0; i < _Mesh->mNumVertices; i++)
		{
			SkinnedMesh vertex;
			memcpy(&vertex.Position, &_Mesh->mVertices[i], sizeof(XMFLOAT3));
			memcpy(&vertex.Normal, &_Mesh->mNormals[i], sizeof(XMFLOAT3));
			if (_Mesh->HasTextureCoords(0))
				memcpy(&vertex.Texture, &_Mesh->mTextureCoords[0][i], sizeof(XMFLOAT2));


			// Bone indices and weights.
			//for (UINT k = 0; k < weights[i].boneIndices.size(); k++)
			//	vertex.BoneIndices[k] = weights[i].boneIndices[k];
			for (UINT k = 0; k < weights[i].boneIndices.size(); k++)
			{
				switch (k)
				{
				case 0:
					vertex.BoneIndices.x = weights[i].boneIndices[k];
					break;
				case 1:
					vertex.BoneIndices.y = weights[i].boneIndices[k];
					break;
				case 2:
					vertex.BoneIndices.z = weights[i].boneIndices[k];
					break;
				case 3:
					vertex.BoneIndices.w = weights[i].boneIndices[k];
					break;
				}
			}


			vertex.Weight.x = weights[i].weights.size() >= 1 ? weights[i].weights[0] : 0;
			vertex.Weight.y = weights[i].weights.size() >= 2 ? weights[i].weights[1] : 0;
			vertex.Weight.z = weights[i].weights.size() >= 3 ? weights[i].weights[2] : 0;
			vertex.Weight.w = weights[i].weights.size() >= 4 ? weights[i].weights[3] : 0;

			vertices.push_back(vertex);

		}

		// 인덱스
		for (unsigned int i = 0; i < _Mesh->mNumFaces; i++)
		{
			const aiFace& Face = _Mesh->mFaces[i];
			assert(Face.mNumIndices == 3);
			indices.push_back(Face.mIndices[0]);
			indices.push_back(Face.mIndices[1]);
			indices.push_back(Face.mIndices[2]);
		}


		ProcessMaterial(mesh, _textures);

		mesh->Initialize(GraphicDevice::GetInstance()->GetDevice(), vertices, indices);

		_meshes.push_back(mesh);
	}
}

vector<WEIGHTS> AssimpLoader::CalculateWeights(aiMesh * _mesh, shared_ptr<AnimationController>* _Animator)
{
	vector<WEIGHTS> weights(_mesh->mNumVertices);

	// Loop through all bones.
	for (UINT i = 0; i < _mesh->mNumBones; i++)
	{
		// Loop through all the vertices the bone affects.
		for (UINT j = 0; j < _mesh->mBones[i]->mNumWeights; j++)
		{
			aiVertexWeight weight = _mesh->mBones[i]->mWeights[j];

			// Get the bone index from the animator by it's name.
			int index = (*_Animator)->GetBoneIndex(_mesh->mBones[i]->mName.C_Str());

			weights[weight.mVertexId].boneIndices.push_back(index);
			weights[weight.mVertexId].weights.push_back(_mesh->mBones[i]->mWeights[j].mWeight);
		}
	}
	return weights;
}

HRESULT AssimpLoader::ProcessMaterial(shared_ptr<MESH> _mesh, vector<shared_ptr<Texture>>& _textures)
{
	string MaterialPath = FolderPath + "\\" + _mesh->MaterialName + ".mat";

	filesystem::path p = MaterialPath;
	if (!filesystem::exists(p))
	{
		return S_OK;
	}

	fstream file;
	file.open(MaterialPath.c_str(), ios::in);

	for (int i = 0; i < 2; ++i)
	{
		TEXTUREDESC texture;

		string buffer;

		getline(file, buffer);
		string MaterialRead;
		MaterialRead.assign(buffer.begin(), buffer.end());

		string TextureType;
		TextureType.assign(MaterialRead.begin(), MaterialRead.begin() + MaterialRead.find("="));
		MaterialRead.erase(MaterialRead.begin(), MaterialRead.begin() + MaterialRead.find("=") + 1);
		MaterialRead = MaterialRead + ".tga";

		string TexturePath = FolderPath + "\\" + MaterialRead;

		//texture.MaterialName = _mesh->MaterialName;
		//texture.Type = TextureType;
		//texture.Path = TexturePath;

		if (TextureType == "Diffuse")
		{
			_mesh->Diffuse = MatchTexture(texture, _textures);

		}
		else if (TextureType == "Normal")
		{
			_mesh->Normal = MatchTexture(texture, _textures);
		}
	}

	file.close();

	return S_OK;
}

shared_ptr<Texture> AssimpLoader::MatchTexture(TEXTUREDESC& _texture, vector<shared_ptr<Texture>>& _textures)
{
	//for (UINT i = 0; i < _textures.size(); ++i)
	//{
	//	if (_textures[i]->GetTextureDecs().Path == _texture.Path)
	//		return _textures[i];
	//}

	auto tex = Texture::Create(GraphicDevice::GetInstance()->GetDevice(), &_texture);
	_textures.push_back(tex);
	return tex;
}

HRESULT AssimpLoader::LoadModel(string _MeshFilePath, vector<shared_ptr<MESH>>& _meshes, vector<shared_ptr<Texture>>& _textures, shared_ptr<AnimationController>* _Animator)
{
	if (FAILED(ReadFile(_MeshFilePath, _meshes, _textures, _Animator)))
	{
		MSG_BOX("Failed to read Model file.");
		return E_FAIL;
	}
	return S_OK;
}
