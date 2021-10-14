#include "pch.h"
#include "AssimpConverter.h"
#include "BinaryFile.h"
#include "Path.h"
#include "String.h"

AssimpConverter::AssimpConverter()
{
	importer = new Assimp::Importer();
}

AssimpConverter::AssimpConverter(const AssimpConverter & Rhs)
{
}

AssimpConverter::~AssimpConverter()
{
	delete importer;
}

void AssimpConverter::ReadFile(wstring _File)
{
	this->File = L"../Resources/" + _File;

	scene = importer->ReadFile
	(
		ToString(this->File),
		aiProcess_ConvertToLeftHanded |     // �޼���ǥ���
		aiProcess_Triangulate		  |		// �ﰢ������ �ٲٱ�
		aiProcess_GenUVCoords		  |		// �� ���Ŀ� �°� UV�ٽ� ���
		aiProcess_GenNormals		  |     // Normal Vector �ٽ� ���
		aiProcess_CalcTangentSpace          // �븻���� �� �� ���
	);

	assert(scene != nullptr);


}

void AssimpConverter::ExportMesh(wstring _SavePath)
{
	_SavePath = L"../Resources/" + _SavePath + L".mesh";

	ReadBoneData(scene->mRootNode, -1, -1);
	WriteMeshData(_SavePath);
}

void AssimpConverter::ReadBoneData(aiNode * _Node, int _Index, int _Parent)
{
	asBone* bone = new asBone();
	bone->Index = _Index;
	bone->Parent = _Parent;
	bone->Name = _Node->mName.C_Str();


	Matrix transform(_Node->mTransformation[0]);
	//bone->transform = XMMatrixTranspose(transform);
	bone->transform = transform;

	Matrix parentMatrix;

	if (_Parent < 0)
		parentMatrix = XMMatrixIdentity();
	else
		parentMatrix = Bones[_Parent]->transform;

	bone->transform = bone->transform * parentMatrix; // ���� �ڱ⸦ �������� �θ�ŭ ������ ����.

	Bones.push_back(bone);

	ReadMeshData(_Node, _Index);

	for (UINT i = 0; i < _Node->mNumChildren; ++i)
	{
		ReadBoneData(_Node->mChildren[i], Bones.size(), _Index);
	}
}

void AssimpConverter::ReadMeshData(aiNode * _Node, int _Bone)
{
	if (_Node->mNumMeshes < 1)
		return;


	for (UINT i = 0; i < _Node->mNumMeshes; ++i)
	{
		asMesh* mesh = new asMesh();
		mesh->Name = _Node->mName.C_Str();
		mesh->BoneIndex = _Bone;

		UINT index = _Node->mMeshes[i];
		aiMesh* srcMesh = scene->mMeshes[index];

		aiMaterial* material = scene->mMaterials[srcMesh->mMaterialIndex];
		mesh->MaterialName = material->GetName().C_Str();

		UINT startVertex = mesh->Vertices.size(); // �޽ð� ������ �̹Ƿ� ������ ��� �̾ �����ϱ����� ������ �׾Ƴ��� ũ�⸦ ����.
		for (UINT v = 0; v < srcMesh->mNumVertices; ++v)
		{
			ModelVertex vertex;
			memcpy(&vertex.Position, &srcMesh->mVertices[v], sizeof(Vector3));
			srcMesh->mBones[0]->mName;
			// �Ϲ������� 0���� ����
			if (srcMesh->HasTextureCoords(0))
				memcpy(&vertex.Uv, &srcMesh->mTextureCoords[0][v], sizeof(Vector2));

			if(srcMesh->HasNormals())
				memcpy(&vertex.Normal, &srcMesh->mNormals[v], sizeof(Vector3));

			mesh->Vertices.push_back(vertex);
		}

		for (UINT f = 0; f < srcMesh->mNumFaces; ++f)
		{
			aiFace& face = srcMesh->mFaces[f];
			for (UINT k = 0; k < face.mNumIndices; ++k)
				mesh->Indices.push_back(face.mIndices[k] + startVertex);
		}

		Meshes.push_back(mesh);

	}

}

void AssimpConverter::ReadSkinData()
{
	for (UINT i = 0; i < scene->mNumMeshes; i++)
	{
		aiMesh* aiMesh = scene->mMeshes[i];
		if (aiMesh->HasBones() == false)
			continue;


		asMesh* mesh = Meshes[i];

		vector<asBoneWeights> boneWeights;
		boneWeights.assign(mesh->Vertices.size(), asBoneWeights());

		for (UINT b = 0; b < aiMesh->mNumBones; b++)
		{
			aiBone* aiMeshBone = aiMesh->mBones[b];

			UINT boneIndex = 0;
			for (asBone* bone : Bones)
			{
				if (bone->Name == (string)aiMeshBone->mName.C_Str())
				{
					boneIndex = bone->Index;

					break;
				}
			}//for(bone)

			for (UINT w = 0; w < aiMeshBone->mNumWeights; w++)
			{
				UINT index = aiMeshBone->mWeights[w].mVertexId;
				float weight = aiMeshBone->mWeights[w].mWeight;

				boneWeights[index].AddWeights(boneIndex, weight);
			}
		}//for(b)


		for (UINT w = 0; w < boneWeights.size(); w++)
		{
			boneWeights[i].Normalize();

			asBlendWeight blendWeight;
			boneWeights[w].GetBlendWeights(blendWeight);

			mesh->Vertices[w].BlendIndices = blendWeight.Indices;
			mesh->Vertices[w].BlendWeights = blendWeight.Weights;
		}
	}
}

void AssimpConverter::WriteMeshData(wstring _SavePath)
{
	Path::CreateFolders(Path::GetDirectoryName(_SavePath));
	
	BinaryWriter* w = new BinaryWriter();

	w->Open(_SavePath);

	w->WriteUInt(Bones.size());
	for (asBone* bone : Bones)
	{
		w->WriteInt(bone->Index);
		w->WriteString(bone->Name);  // String�� ���ٸ� ����ü byte������ �а� ���� ���� ����.
		w->WriteInt(bone->Parent);
		w->WriteMatrix(bone->transform);

		SAFEDELETE(bone);
	}

	w->WriteUInt(Meshes.size());
	for (asMesh* meshData : Meshes)
	{
		w->WriteString(meshData->Name);
		w->WriteInt(meshData->BoneIndex);

		w->WriteString(meshData->MaterialName);

		w->WriteUInt(meshData->Vertices.size());
		w->WriteByte(&meshData->Vertices[0], sizeof(ModelVertex) * meshData->Vertices.size());

		w->WriteUInt(meshData->Indices.size());
		w->WriteByte(&meshData->Indices[0], sizeof(UINT) * meshData->Indices.size());

		SAFEDELETE(meshData);
	}

	w->Close();
	SAFEDELETE(w);
}
