#pragma once

#include "Includes.h"


static const XMFLOAT3 vector3Epsilon = XMFLOAT3(0.00001f, 0.00001f, 0.00001f); 
static const XMFLOAT2 vector2Epsilon = XMFLOAT2(0.00001f, 0.00001f);
static const XMFLOAT3 vector3True;
static const XMFLOAT2 vector2True;

bool CompareVector2WithEpsilon(const XMFLOAT2& lhs, const XMFLOAT2& rhs);
bool CompareVector3WithEpsilon(const XMFLOAT3& lhs, const XMFLOAT3& rhs);
FbxAMatrix GetGeometryTransformation(FbxNode* inNode);

struct VertexBlendingInfo
{
	unsigned int mBlendingIndex;
	double mBlendingWeight;

	VertexBlendingInfo() :
		mBlendingIndex(0),
		mBlendingWeight(0.0)
	{}

	bool operator < (const VertexBlendingInfo& rhs)
	{
		return (mBlendingWeight > rhs.mBlendingWeight);
	}
};

struct ModelVertex
{
	Vector3 Position;
	Vector2 Uv;
	Vector3 Normal;
	Vector4 Tangent;
	Vector4 BlendWeights;
	Vector4 BlendIndices;
};

struct PNTIWVertex
{
	XMFLOAT3 mPosition;
	XMFLOAT3 mNormal;
	XMFLOAT2 mUV;
	std::vector<VertexBlendingInfo> mVertexBlendingInfos;

	void SortBlendingInfoByWeight()
	{
		std::sort(mVertexBlendingInfos.begin(), mVertexBlendingInfos.end());
	}

	bool operator==(const PNTIWVertex& rhs) const
	{
		bool sameBlendingInfo = true;

		// We only compare the blending info when there is blending info
		if (!(mVertexBlendingInfos.empty() && rhs.mVertexBlendingInfos.empty()))
		{
			// Each vertex should only have 4 index-weight blending info pairs
			for (unsigned int i = 0; i < 4; ++i)
			{
				if (mVertexBlendingInfos[i].mBlendingIndex != rhs.mVertexBlendingInfos[i].mBlendingIndex ||
					abs(mVertexBlendingInfos[i].mBlendingWeight - rhs.mVertexBlendingInfos[i].mBlendingWeight) > 0.001)
				{
					sameBlendingInfo = false;
					break;
				}
			}
		}

		bool result1 = CompareVector3WithEpsilon(mPosition, rhs.mPosition);
		bool result2 = CompareVector3WithEpsilon(mNormal, rhs.mNormal);
		bool result3 = CompareVector2WithEpsilon(mUV, rhs.mUV);

		return result1 && result2 && result3 && sameBlendingInfo;
	}
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
	wstring	Path;	// 다른 첵스쳐와 비교하기 위해 경로 저장.
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
		// boneWeights가 큰 값이 앞에 있도록 넣기. 3D모델의 규칙.

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

	// Weights 총합 1로 만들기
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
//실제로 파일로 저장될 데이터 **중요**
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

// 애니메이션의 한 동작을 의미
struct asClip
{
	string Name;

	UINT FrameCount;
	float FrameRate;
	float Duration;

	vector<asKeyframe *> Keyframes;
};
//////////////////////////////////////////////////////////////


//aniNode의 원본 키프레임 저장.
struct asClipNode
{
	// Bone의 이름
	aiString Name;

	// 해당 본의 SRT값들
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

// 하나의 클립.
struct ClipTransform
{
	Matrix** Transform;

	ClipTransform()
	{
		// 행은 keyframe갯수 만큼 동적할당.
		Transform = new Matrix*[MAX_MODEL_KEYFRAMES];

		// 열 별로 동적할당.
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

// 프레임 정보에 대한 구조체. 셰이더 쪽으로 넘겨줄 거임
struct KeyframeDesc
{
	// 현재 플레이하려는 애니메이션의 번호.
	int clip = 0;

	// 현재 프레임 번호.
	UINT CurrFrame = 0;
	// 현재 프레임의 다음 번호. CurrFrame의 +1
	UINT NextFrame = 0;

	// 현재 플레이중인 시간 0~1로 정규화 될 거임.
	float Time = 0.0f;
	// 애니메이션이 시작했을 때 부터의 시간.
	float RunningTime = 0.0f;

	// 애니메이션 속도.
	float Speed = 1.0f;

	Vector2 Padding;
};


struct BlendingIndexWeightPair
{
	unsigned int mBlendingIndex;
	double mBlendingWeight;

	BlendingIndexWeightPair() :
		mBlendingIndex(0),
		mBlendingWeight(0)
	{}
};

// Each Control Point in FBX is basically a vertex
// in the physical world. For example, a cube has 8
// vertices(Control Points) in FBX
// Joints are associated with Control Points in FBX
// The mapping is one joint corresponding to 4
// Control Points(Reverse of what is done in a game engine)
// As a result, this struct stores a XMFLOAT3 and a 
// vector of joint indices
struct CtrlPoint
{
	XMFLOAT3 mPosition;
	std::vector<BlendingIndexWeightPair> mBlendingInfo;

	CtrlPoint()
	{
		mBlendingInfo.reserve(4);
	}
};


// This stores the information of each key frame of each joint
// This is a linked list and each node is a snapshot of the
// global transformation of the joint at a certain frame
struct Keyframe
{
	FbxLongLong mFrameNum;
	FbxAMatrix mGlobalTransform;
	Keyframe* mNext;

	Keyframe() :
		mNext(nullptr)
	{}
};

// This is the actual representation of a joint in a game engine
struct Joint
{
	string mName;
	int mParentIndex;
	FbxAMatrix mGlobalBindposeInverse;
	Keyframe* mAnimation;
	FbxNode* mNode;

	Joint() :
		mNode(nullptr),
		mAnimation(nullptr)
	{
		mGlobalBindposeInverse.SetIdentity();
		mParentIndex = -1;
	}

	~Joint()
	{
		while (mAnimation)
		{
			Keyframe* temp = mAnimation->mNext;
			delete mAnimation;
			mAnimation = temp;
		}
	}
};

typedef struct tagSkeleton
{
	vector<Joint> Joints;
}SEKELETON;

typedef struct tagTriangle
{
	vector<unsigned int> mIndices;
	string mMaterialName;
	UINT mMaterialIndex;

	bool operator<(const tagTriangle& rhs)
	{
		return mMaterialIndex < rhs.mMaterialIndex;
	}
}TRIANGLE;



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
