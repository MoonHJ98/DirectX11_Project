#pragma once

#include "Includes.h"

class Bone;
class VertexBuffer;
class IndexBuffer;

class Mesh
{
public:
	Mesh();
	Mesh(const Mesh& Rhs);

public:
	~Mesh();

private:
	HRESULT Initialize(wstring _name, int _boneIndex, wstring _materialName, shared_ptr<ModelVertex> _vertices, UINT _vertexCount, shared_ptr<UINT> _indices, UINT _indexCount);

public:
	//void Pass(UINT val) { pass = val; }
	//void SetShader(Shader* shader);

	void Update();
	void Render();

	wstring GetName() { return name; }

	int GetBoneIndex() { return boneIndex; }
	shared_ptr<Bone> GetBone() { return bone; }
	void SetBone(shared_ptr<Bone> _bone) { bone = _bone; }

	//void GetTransforms(Matrix* transforms);
	//void SetTransform(Transform* transform);

private:
	struct BoneDesc
	{
		Matrix Transforms[MAX_MODEL_TRANSFORMS];

		UINT Index;
		float Padding[3];
	} boneDesc;


public:
	static shared_ptr<Mesh> Create(wstring _name, int _boneIndex, wstring _materialName, shared_ptr<ModelVertex> _vertices, UINT _vertexCount, shared_ptr<UINT> _indices, UINT _indexCount);

private:
	wstring name;
	wstring materialName = L"";
	int boneIndex;
	shared_ptr<Bone> bone = nullptr;

	shared_ptr<VertexBuffer> vertexBuffer = nullptr;
	UINT vertexCount;
	shared_ptr<ModelVertex> vertices = nullptr;

	shared_ptr<IndexBuffer> indexBuffer = nullptr;
	UINT indexCount;
	shared_ptr<UINT> indices = nullptr;

	//Transform* transform = nullptr;
	//Shader* shader;
	//PerFrame* perFrame = NULL;
	//ConstantBuffer* boneBuffer;
	//ID3DX11EffectConstantBuffer* sBoneBuffer;
};

