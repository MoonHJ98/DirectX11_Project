#pragma once

#include "Includes.h"

class Bone;
class GraphicDevice;
class Texture;

class Mesh
{
public:
	Mesh();
	Mesh(const Mesh& Rhs);

public:
	~Mesh();

private:
	HRESULT				Initialize(wstring _name, int _boneIndex, wstring _materialName, vector<ModelVertex>& _vertices, vector<UINT>& _indices);

public:
	void				Update();
	void				Render();

public:
	wstring				GetName()								 { return name; }
	wstring				GetMaterialName()						 { return materialName; }
																 
	int					GetBoneIndex()							 { return boneIndex; }
	shared_ptr<Bone>	GetBone()								 { return bone; }
	void				SetBone(shared_ptr<Bone> _bone)			 { bone = _bone; }

	void				SetDiffuse(shared_ptr<Texture> _Diffuse) { Diffuse = _Diffuse; }
	void				SetNormal(shared_ptr<Texture> _Normal)	 { Normal = _Normal; }
	

	void				SetTransforms(Matrix* transforms);
	//void GetTransforms(Matrix* transforms);
	//void SetTransform(Transform* transform);

private:
	struct BoneDesc
	{
		Matrix Transforms[MAX_MODEL_TRANSFORMS];

		UINT Index;
		float Padding[3];
	};


public:
	static shared_ptr<Mesh> Create(wstring _name, int _boneIndex, wstring _materialName, vector<ModelVertex>& _vertices, vector<UINT>& _indices);

private:
	shared_ptr<GraphicDevice> Graphic = nullptr;
	wstring name;
	wstring materialName = L"";
	int boneIndex;
	shared_ptr<Bone> bone = nullptr;

	ComPtr<ID3D11Buffer> vertexBuffer;
	ComPtr<ID3D11Buffer> indexBuffer;
	UINT vertexCount;
	UINT indexCount;

	shared_ptr<ModelVertex> vertices = nullptr;
	shared_ptr<UINT> indices = nullptr;


	BoneDesc boneDesc;
	ConstantBuffer<BoneDesc> BoneMatrixbuffer;

	shared_ptr<Texture> Diffuse;
	shared_ptr<Texture> Normal;

	//Transform* transform = nullptr;
	//Shader* shader;
	//PerFrame* perFrame = NULL;
	//ConstantBuffer* boneBuffer;
	//ID3DX11EffectConstantBuffer* sBoneBuffer;


};

