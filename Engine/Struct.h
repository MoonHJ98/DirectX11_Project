#pragma once

#include "Includes.h"


struct ModelVertex
{
	Vector3 Position;
	Vector2 Uv;
	Vector3 Normal;
	Vector3 Tangent;
	Vector3 BiTangent;
	Vector4 BlendWeights;
	Vector4 BlendIndices;
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
	XMMATRIX lightViewMatrix = {};
	XMMATRIX lightProjectionMatrix = {};
	BOOL renderDepthForShadow;
	BOOL padding[3];


}MATRIXBUFFERTYPE;


typedef struct tagTexture
{
	wstring MaterialName;
	wstring	Type;
	wstring	Path;	// �ٸ� ý���Ŀ� ���ϱ� ���� ��� ����.
}TEXTUREDESC;

typedef struct tagMaterial
{
	wstring name;
	Color Ambient = Color();
	Color Diffuse = Color();
	Color Specular = Color();
	Color Emissive = Color();

	wstring DiffuseMap = L"";
	wstring SpecularMap = L"";
	wstring NormalMap = L"";


}MATERIALDESC;

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

struct asMaterial
{
	string Name;

	Color Ambient;
	Color Diffuse;
	Color Specular;
	Color Emissive;

	string DiffuseFile;
	string SpecularFile;
	string NormalFile;
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

// ������ ������ ���� ����ü. ���̴� ������ �Ѱ��� ����
struct KeyframeDesc
{
	// ���� �÷����Ϸ��� �ִϸ��̼��� ��ȣ.
	int clip = 0;

	// ���� ������ ��ȣ.
	UINT CurrFrame = 0;
	// ���� �������� ���� ��ȣ. CurrFrame�� +1
	UINT NextFrame = 0;

	// ���� �÷������� �ð� 0~1�� ����ȭ �� ����.
	float Time = 0.0f;
	// �ִϸ��̼��� �������� �� ������ �ð�.
	float RunningTime = 0.0f;

	// �ִϸ��̼� �ӵ�.
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

typedef struct LightDesc {
	D3DLIGHTTYPE    Type;						  /* Type of light source */
	Color			Diffuse = Color();			  /* Diffuse color of light */
	Color			Specular = Color();			  /* Specular color of light */
	Color			Ambient = Color();			  /* Ambient color of light */
	Vector3         Position = Vector3();         /* Position in world space */
	Vector3         Direction = Vector3();        /* Direction in world space */
	D3DVALUE        SpecularPower = 0.f;          /* Specular Power */
	D3DVALUE        Range = 0.f;				  /* Cutoff range */
	D3DVALUE        dvFalloff = 0.f;			  /* Falloff */
	D3DVALUE        dvAttenuation0 = 0.f;		  /* Constant attenuation */
	D3DVALUE        dvAttenuation1 = 0.f;		  /* Linear attenuation */
	D3DVALUE        dvAttenuation2 = 0.f;		  /* Quadratic attenuation */
	D3DVALUE        dvTheta = 0.f;				  /* Inner angle of spotlight cone */
	D3DVALUE        dvPhi = 0.f;				  /* Outer angle of spotlight cone */
}LIGHTDESC;

typedef struct ColliderDesc
{
	Vector3 scale;
	float radius;
	float halfHeight;
}COLLIDERDESC;

struct NodeType
{
	float positionX = 0.f;
	float positionZ = 0.f;
	float width = 0.f;
	int triangleCount = 0;
	int indexCount = 0;
	ComPtr<ID3D11Buffer> vertexBuffer;
	ComPtr<ID3D11Buffer> indexBuffer;
	shared_ptr<NodeType> nodes[4] = { NULL };

};

struct BrushDesc
{
	Color color = Color(0.f, 1.f, 0.f, 1.f);
	Vector3 position = Vector3(0.f, 0.f, 0.f);
	UINT range = 5;
};

enum ComponentType { COLLIDER, TRANSFORM, SHADER, MATERIAL, RIGIDBODY, TERRAIN, COMPONENT_END };
enum RigidbodyType { DYNAMICRIGID, STATICRIGID, RIGIDBODY_END };

struct FilterGroup
{
	enum Enum
	{
		eSUBMARINE = (1 << 0),
		eMINE_HEAD = (1 << 1),
		eMINE_LINK = (1 << 2),
		eCRAB = (1 << 3),
		eHEIGHTFIELD = (1 << 4),
	};
};

struct TerrainVertexType
{
	Vector3 position;
	Vector3 normal;
	Vector2 Uv;
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

