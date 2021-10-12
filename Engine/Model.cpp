#include "pch.h"
#include "Model.h"
#include "AssimpConverter.h"
#include "Bone.h"
#include "Mesh.h"
#include "BinaryFile.h"

Model::Model()
{
	//a = new AssimpConverter();
	//a->ReadFile(L"Player/SK_CHR_Jack.FBX");
	//a->ExportMesh(L"Player/SK_CHR_Jack.FBX");
	ReadMesh(L"Player/SK_CHR_Jack.FBX");
}

Model::~Model()
{
}

void Model::ReadMesh(wstring filePath)
{
	filePath = L"../Resources/" + filePath + L".mesh";

	BinaryReader* r = new BinaryReader();
	r->Open(filePath);
	
	UINT count = 0;
	
	// Bone.
	count = r->ReadUInt();
	for (UINT i = 0; i < count; ++i)
	{
		int index = r->ReadInt();
		wstring name = ToWString(r->ReadString());
		int parentIndex = r->ReadInt();
		Matrix transform = r->ReadMatrix();
		bones.push_back(Bone::Create(index, name, parentIndex, transform));
	}

	// Mesh.
	count = r->ReadUInt();
	for (UINT i = 0; i < count; ++i)
	{
		wstring name = ToWString(r->ReadString());
		int boneIndex = r->ReadInt();
		wstring materialName = ToWString(r->ReadString());
		shared_ptr<ModelVertex> MeshVertices = nullptr;
		UINT VertexCount = 0;
		shared_ptr<UINT> MeshIndices = nullptr;
		UINT IndexCount = 0;

		// Vertex.
		{
			UINT count = r->ReadUInt();
			vector<ModelVertex> vertices;
			vertices.assign(count, ModelVertex());
	
			void* ptr = (void*)&(vertices[0]);
			r->ReadByte(&ptr, sizeof(ModelVertex)* count);
	
			shared_ptr<ModelVertex> tempVertices(new ModelVertex[count]);
			VertexCount = count;
			copy(vertices.begin(), vertices.end(), tempVertices.get());
			MeshVertices = tempVertices;
	
		}
		// Index
		{
			UINT count = r->ReadUInt();
			vector<UINT> indices;
			indices.assign(count, UINT());

			void* ptr = (void*)&(indices[0]);
			r->ReadByte(&ptr, sizeof(UINT) * count);
			shared_ptr<UINT> tempIndices(new UINT[count]);
			IndexCount = count;
			copy(indices.begin(), indices.end(), tempIndices.get());
			MeshIndices = tempIndices;
		}
		meshes.push_back(Mesh::Create(name, boneIndex, materialName, MeshVertices, VertexCount, MeshIndices, IndexCount));
	}

	r->Close();
	SAFEDELETE(r);

	BindBone();
	//BindMesh();
	
}

void Model::BindBone()
{
	Root = bones[0];
	for (auto& bone : bones)
	{
		if (bone->GetParentIndex() > -1)
		{
			bone->SetParentBone(bones[bone->GetParentIndex()]);
			bone->GetParentBone()->GetChilds().push_back(bone);
		}
		else
			bone->SetParentBone(nullptr);
	}
}

void Model::BindMesh()
{
	for (auto& mesh : meshes)
	{
		mesh->SetBone(bones[mesh->GetBoneIndex()]);

	}
}
