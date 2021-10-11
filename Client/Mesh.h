#pragma once

#include "Includes.h"

class Transform;
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
	HRESULT Initialize();

public:
	void Pass(UINT val) { pass = val; }
	//void SetShader(Shader* shader);

	void Update();
	void Render();

	wstring Name() { return name; }

	int BoneIndex() { return boneIndex; }
	class ModelBone* Bone() { return bone; }

	void Transforms(Matrix* transforms);
	//void SetTransform(Transform* transform);

private:
	struct BoneDesc
	{
		Matrix Transforms[MAX_MODEL_TRANSFORMS];

		UINT Index;
		float Padding[3];
	} boneDesc;

public:
	static shared_ptr<Mesh> Create();

private:
	wstring name;

	//Shader* shader;
	UINT pass = 0;

	shared_ptr<Transform> transform = nullptr;
	//PerFrame* perFrame = NULL;

	wstring materialName = L"";


	int boneIndex;
	class ModelBone* bone;

	shared_ptr<VertexBuffer> vertexBuffer = nullptr;
	UINT vertexCount;
	ModelVertex* vertices;

	shared_ptr<IndexBuffer> indexBuffer = nullptr;
	UINT indexCount;
	UINT* indices;

	//ConstantBuffer* boneBuffer;
	//ID3DX11EffectConstantBuffer* sBoneBuffer;
};

