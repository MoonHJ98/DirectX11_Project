#pragma once

#include "Includes.h"

struct ModelVertex
{
	Vector3 Position;
	Vector2 Uv;
	Vector3 Normal;
	Vector4 Tangent;
	Vector4 BlendIndices;
	Vector4 BlendWeights;
};

typedef struct MatrixBufferType
{
	MatrixBufferType() {}
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

typedef struct tagTexture
{
	wstring MaterialName;
	wstring	Type;
	wstring	Path;	// �ٸ� ý���Ŀ� ���ϱ� ���� ��� ����.
}TEXTUREDESC;

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
		// boneWeights�� ū ���� �տ� �ֵ��� �ֱ�. 3D���� ��Ģ.

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

	// Weights ���� 1�� �����
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


// For Assimp.
//������ ���Ϸ� ����� ������ **�߿�**
//////////////////////////////////////////////////////////////
struct asKeyframeData
{
	float Time;

	Vector3 Scale;
	Quaternion Rotation;
	Vector3 Translation;
};


struct asKeyframe
{
	string BoneName;
	vector<asKeyframeData> Transforms;
};

// �ִϸ��̼��� �� ������ �ǹ�
struct asClip
{
	string Name;

	UINT FrameCount;
	float FrameRate;
	float Duration;

	vector<asKeyframe *> Keyframes;
};
//////////////////////////////////////////////////////////////


//aniNode�� ���� Ű������ ����.
struct asClipNode
{
	// Bone�� �̸�
	aiString Name;

	// �ش� ���� SRT����
	vector<asKeyframeData> Keyframe; 
};

// For Model.
struct ModelKeyframeData
{
	float Time;

	Vector3 Scale;
	Quaternion Rotation;
	Vector3 Translation;
};

struct ModelKeyframe
{
	wstring BoneName;
	vector<ModelKeyframeData> Transforms;
};

// �ϳ��� Ŭ��.
struct ClipTransform
{
	Matrix** Transform;

	ClipTransform()
	{
		// ���� keyframe���� ��ŭ �����Ҵ�.
		Transform = new Matrix*[MAX_MODEL_KEYFRAMES];

		// �� ���� �����Ҵ�.
		for (UINT i = 0; i < MAX_MODEL_KEYFRAMES; i++)
			Transform[i] = new Matrix[MAX_MODEL_TRANSFORMS];
	}

	~ClipTransform()
	{
		for (UINT i = 0; i < MAX_MODEL_KEYFRAMES; i++)
			SAFEDELETEARRAY(Transform[i]);

		SAFEDELETEARRAY(Transform);
	}
};


struct KeyframeDesc
{
	int Clip = 0;

	UINT CurrFrame = 0;
	UINT NextFrame = 0;

	float Time = 0.0f;
	float RunningTime = 0.0f;

	float Speed = 1.0f;

	Vector2 Padding;
};


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
