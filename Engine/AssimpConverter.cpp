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
		aiProcess_ConvertToLeftHanded |     // 왼손좌표계로
		aiProcess_Triangulate		  |		// 삼각형으로 바꾸기
		aiProcess_GenUVCoords		  |		// 모델 형식에 맞게 UV다시 계산
		aiProcess_GenNormals		  |     // Normal Vector 다시 계산
		aiProcess_CalcTangentSpace          // 노말맵핑 할 때 사용
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

	bone->transform = bone->transform * parentMatrix; // 본을 자기를 기준으로 부모만큼 움직일 것임.

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

		UINT startVertex = mesh->Vertices.size(); // 메시가 여러개 이므로 정점을 계속 이어서 저장하기위해 이전의 쌓아놓은 크기를 저장.
		for (UINT v = 0; v < srcMesh->mNumVertices; ++v)
		{
			ModelVertex vertex;
			memcpy(&vertex.Position, &srcMesh->mVertices[v], sizeof(Vector3));
			srcMesh->mBones[0]->mName;
			// 일반적으로 0으로 고정
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
		w->WriteString(bone->Name);  // String이 없다면 구조체 byte단위로 읽고 쓰는 것이 빠름.
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
