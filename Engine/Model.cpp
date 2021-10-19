#include "pch.h"
#include "Model.h"
#include "AssimpConverter.h"
#include "GraphicDevice.h"
#include "Bone.h"
#include "Mesh.h"
#include "Animator.h"
#include "BinaryFile.h"
#include "Shader.h"
#include "Texture.h"
#include "Transform.h"
#include "Management.h"

Model::Model()
{
}

Model::Model(const Model & Rhs)
{
}

Model::~Model()
{
}

HRESULT Model::Initialize()
{
	AssimpConverter* converter = nullptr;;

	converter = new AssimpConverter();
	converter->ReadFile(L"Player/SK_CHR_Jack.FBX");
	converter->ExportMesh(L"Player/SK_CHR_Jack");
	SAFEDELETE(converter);

	converter = new AssimpConverter();
	converter->ReadFile(L"Player/Jack_CB_IDL_01_Lp.FBX");
	converter->ExportAnimClip(0, L"Player/Jack_CB_IDL_01_Lp");
	SAFEDELETE(converter);

	ReadMesh(L"Player/SK_CHR_Jack");
	ReadMaterial();
	shader = Shader::Create(L"../Engine/ModelVS.hlsl", L"../Engine/ModelPS.hlsl");

	animator = Animator::Create(shared_from_this());

	animator->ReadClip(L"Player/Jack_CB_IDL_01_Lp");

	trans = XMMatrixIdentity();

	return S_OK;
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
		vector<ModelVertex> vertices;
		vector<UINT> indices;
		// Vertex.
		{
			UINT count = r->ReadUInt();
			
			vertices.assign(count, ModelVertex());
	
			void* ptr = (void*)&(vertices[0]);
			r->ReadByte(&ptr, sizeof(ModelVertex)* count);
		}
		// Index
		{
			UINT count = r->ReadUInt();
			
			indices.assign(count, UINT());

			void* ptr = (void*)&(indices[0]);
			r->ReadByte(&ptr, sizeof(UINT) * count);
		}
		meshes.push_back(Mesh::Create(name, boneIndex, materialName, vertices, indices));
	}

	r->Close();
	SAFEDELETE(r);

	BindBone();
	BindMesh();
	
}

void Model::ReadMaterial()
{
	for (UINT i = 0; i < meshes.size(); ++i)
	{
		wstring MaterialPath = L"../Resources/Player/" + meshes[i]->GetMaterialName() + L".mat";
		wfstream file;
		file.open(MaterialPath.c_str(), ios::in);
		
		if (!file.is_open())
			continue;

		for (int j = 0; j < 2; ++j)
		{
			TEXTUREDESC textureDesc;

			wstring MaterialRead;

			getline(file, MaterialRead);
			wstring TextureType;
			TextureType.assign(MaterialRead.begin(), MaterialRead.begin() + MaterialRead.find(L"="));
			MaterialRead.erase(MaterialRead.begin(), MaterialRead.begin() + MaterialRead.find(L"=") + 1);
			wstring TextureName;
			TextureName = MaterialRead + L".tga";

			textureDesc.MaterialName = meshes[i]->GetMaterialName();
			textureDesc.Type = TextureType;
			textureDesc.Path = L"../Resources/Player/" + TextureName;

			if (TextureType == L"Diffuse")
			{
				meshes[i]->SetDiffuse(MatchTexture(textureDesc));

			}
			else if (TextureType == L"Normal")
			{
				meshes[i]->SetNormal(MatchTexture(textureDesc));
			}
		}
		file.close();

	}
}

shared_ptr<Texture> Model::MatchTexture(TEXTUREDESC& _texture)
{
	for (UINT i = 0; i < textures.size(); ++i)
	{
		if (textures[i]->GetTextureDecs().Path == _texture.Path)
			return textures[i];
	}

	auto tex = Texture::Create(GraphicDevice::GetInstance()->GetDevice(), &_texture);
	textures.push_back(tex);
	return tex;
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

void Model::UpdateTransform(shared_ptr<Bone> bone, const Matrix & matrix)
{
	if (bone != nullptr)
		UpdateBones(bone, matrix);

	for (UINT i = 0; i < bones.size(); i++)
	{
		auto& bone = bones[i];

		transforms[i] = *bone->GetTransform();
	}

	for (auto& mesh : meshes)
		mesh->SetTransforms(transforms); // 본의 transforms를 메쉬에 넣어주기
}

void Model::UpdateBones(shared_ptr<Bone> bone, const Matrix & matrix)
{
	Matrix temp = *bone->GetTransform();
	Matrix a = temp * matrix;
	bone->SetTransform(a);

	for (auto& bone : bone->GetChilds())
		UpdateBones(bone, matrix);
}

void Model::SetTransformsSRV(ComPtr<ID3D11ShaderResourceView> srv)
{
	for (auto& mesh : meshes)
		mesh->SetTransformsSRV(srv);
}

int Model::Update(float _timeDelta)
{
	animator->Update(_timeDelta);

	//rad = sinf(100.f) * _timeDelta * 10.f;
	//trans = XMMatrixRotationRollPitchYaw(0.f, rad, 0.f);

	UpdateTransform(/*Root, trans*/);

	for (auto& mesh : meshes)
		mesh->Update();

	return 0;
}

void Model::Render()
{
	animator->Render();
	shader->Render();

	for (auto& mesh : meshes)
		mesh->Render();

}

shared_ptr<Model> Model::Create()
{
	shared_ptr<Model> Instance(new Model());
	

	if (FAILED(Instance->Initialize()))
	{
		MSG_BOX("Failed to create Model.");
		return nullptr;
	}
	return Instance;
}
