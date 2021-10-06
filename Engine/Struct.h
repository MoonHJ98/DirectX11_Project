#pragma once

#include "Includes.h"

enum SCENEID { STATIC, LOGO, SCENE_END };


struct SkinnedMesh
{
	XMFLOAT3 Position;
	XMFLOAT2 Texture;
	XMFLOAT3 Normal;
	XMFLOAT4 Weight;
	XMUINT4 BoneIndices;
};

typedef struct MatrixBufferType
{
	MatrixBufferType(XMMATRIX _World, XMMATRIX _View, XMMATRIX _Proj)
	{
		World = _World;
		View = _View;
		Projection = _Proj;
	}
	XMMATRIX World = {};
	XMMATRIX View = {};
	XMMATRIX Projection = {};

}MATRIXBUFFERTYPE;

typedef struct Weights
{
	XMFLOAT4 Weight;
	XMUINT4 BoneIndices;

	vector<int>		boneIndices;
	vector<float>	weights;
}WEIGHTS;

typedef struct BoneEntry
{
	string Name = {};
	XMFLOAT4X4 Offset = {}, LocalTransform = {}, GlobalTransform = {}, OriginalLocalTransform = {};
	BoneEntry* Parent = nullptr;
	vector<BoneEntry*> Children;

	BoneEntry() {}
	~BoneEntry()
	{
		for (size_t i(0); i < Children.size(); i++)
			delete Children[i];
	}
}BONE;

typedef struct tagTexture
{
	string MaterialName;
	string	Type;
	string	Path;	// 다른 첵스쳐와 비교하기 위해 경로 저장.
}TEXTUREDESC;


class Texture;
typedef struct MeshEntry
{
	MeshEntry() {}

	~MeshEntry() {}

	void Initialize(ID3D11Device* _Device, vector<SkinnedMesh>& Vertices, const vector<UINT>& Indices);

	// For Buffer.
	ComPtr<ID3D11Buffer> VertexBuffer;
	ComPtr<ID3D11Buffer> IndexBuffer;
	UINT NumIndices = 0;
	string MaterialName;

	shared_ptr<Texture> Diffuse;
	shared_ptr<Texture> Normal;
	string Name;


	void	RenderBuffer(ID3D11DeviceContext * _DeviceContext);

}MESH;

struct AnimationChannel
{
	string Name;
	vector<aiVectorKey> PositionKeys;
	vector<aiQuatKey> RotationKeys;
	vector<aiVectorKey> ScalingKeys;
};

struct AnimEvaluator
{
	AnimEvaluator() {}
	AnimEvaluator(const aiAnimation* pAnim);
	void Evaluate(float pTime, map<string, BONE*>& bones);
	//void Save(ofstream& file);
	//void Load(ifstream& file);

	vector<XMFLOAT4X4>& GetTransforms(float dt);

	unsigned int GetFrameIndexAt(float _time);

	string Name;
	vector<AnimationChannel> Channels;
	bool PlayAnimationForward = true; // play forward == true, play backward == false
	float LastTime = 0.f, TicksPerSecond = 0.f, Duration = 0.f;
	vector<tuple<UINT, UINT, UINT> > LastPositions;
	vector<vector<XMFLOAT4X4>> Transforms;//, QuatTransforms;/** Array to return transformations results inside. */
};

void TransformMatrix(XMFLOAT4X4& out, const aiMatrix4x4& in);