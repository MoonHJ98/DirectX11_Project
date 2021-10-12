#pragma once

#include "Includes.h"


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

typedef struct BoneTransformBufferType
{
	XMFLOAT4X4 BoneTransform[MAX_BONE_TRANSFORM] = {};

}BONETRASFORMBUFFERTYPE;

typedef struct asWeights
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
	ComPtr<ID3D11Buffer> VertexBuffers;
	ComPtr<ID3D11Buffer> IndexBuffers;
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


///////////////////////////////////////////////////////////


void SplitString(vector<string>* result, string origin, string tok);
void SplitString(vector<wstring>* result, wstring origin, wstring tok);

bool StartsWith(string str, string comp);
bool StartsWith(wstring str, wstring comp);

bool Contain(string str, string comp);
bool Contain(wstring str, wstring comp);

void Replace(string* str, string comp, string rep);
void Replace(wstring* str, wstring comp, wstring rep);

wstring ToWString(string value);
string ToString(wstring value);

string Format(const string format, ...);
wstring Format(const wstring format, ...);

typedef VertexPositionNormalTangentColorTextureSkinning ModelVertex;


struct asBone
{
	int Index;
	string Name;

	int Parent;
	Matrix transform;
};

struct asMesh
{
	string Name;
	int BoneIndex;

	aiMesh* Mesh;

	string MaterialName;

	vector<ModelVertex> Vertices;
	vector<UINT> Indices;
};

struct asBlendWeight
{

	Vector4 Indices = Vector4(0, 0, 0, 0);
	Vector4 Weights = Vector4(0, 0, 0, 0);

	void Set(UINT index, UINT boneIndex, float weight)
	{
		float i = (float)boneIndex;
		float w = weight;

		switch (index)
		{
		case 0: Indices.x = i; Weights.x = w; break;
		case 1: Indices.y = i; Weights.y = w; break;
		case 2: Indices.z = i; Weights.z = w; break;
		case 3: Indices.w = i; Weights.w = w; break;
		}
	}
};

struct asBoneWeights
{
private:
	typedef pair<int, float> Pair;
	vector<Pair> BoneWeights;

public:
	void AddWeights(UINT boneIndex, float boneWeights)
	{
		if (boneWeights <= 0.0f) return;

		bool bInsert = false;
		vector<Pair>::iterator it = BoneWeights.begin();
		while (it != BoneWeights.end())
		{
			if (boneWeights > it->second)
			{
				BoneWeights.insert(it, Pair(boneIndex, boneWeights));
				bInsert = true;

				break;
			}

			it++;
		} // while(it)

		if (bInsert == false)
			BoneWeights.push_back(Pair(boneIndex, boneWeights));
	}

	void GetBlendWeights(asBlendWeight& blendWeights)
	{
		for (UINT i = 0; i < BoneWeights.size(); i++)
		{
			if (i >= 4) return;

			blendWeights.Set(i, BoneWeights[i].first, BoneWeights[i].second);
		}
	}

	void Normalize()
	{
		float totalWeight = 0.0f;

		int i = 0;
		vector<Pair>::iterator it = BoneWeights.begin();
		while (it != BoneWeights.end())
		{
			if (i < 4)
			{
				totalWeight += it->second;
				i++; it++;
			}
			else
				it = BoneWeights.erase(it);
		}

		float scale = 1.0f / totalWeight;

		it = BoneWeights.begin();
		while (it != BoneWeights.end())
		{
			it->second *= scale;
			it++;
		}
	}
};


void TransformMatrix(XMFLOAT4X4& out, const aiMatrix4x4& in);