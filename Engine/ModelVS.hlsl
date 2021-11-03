#define MAX_MODEL_TRANSFORMS 250
#define MAX_MODEL_KEYFRAMES 500
Texture2DArray TransformsMap : register(t0);


struct VertexModel
{
    float3 Position : POSITION;
    float2 Uv : TEXCOORD;
    float3 Normal : NORMAL;
    float3 Tangent : TANGENT;
    float3 BiTangent : BITANGENT;
    float4 BlendWeights : BLENDWEIGHTS;
    float4 BlendIndices : BLENDINDICES;

};

struct VertexOutput
{
    float4 Position : SV_POSITION;
    float2 Uv : TEXCOORD;
    float3 Normal : NORMAL;
    float3 Tangent : TANGENT;
    float3 BiTangent : BITANGENT;
    float4 BlendWeights : BLENDWEIGHTS;
    float4 BlendIndices : BLENDINDICES;  

    float4 WorldPos : TEXCOORD1;
    float4 ProjPos : TEXCOORD2;

};

struct AnimationFrame
{
    int Clip;

    uint CurrFrame;
    uint NextFrame;

    float Time;
    float Running;

    float3 Padding;
};


cbuffer MatrixBuffer : register(b0)
{
    matrix WorldMatrix;
    matrix ViewMatrix;
    matrix ProjectionMatrix;
}

cbuffer CB_Bone : register(b1)
{
    matrix BoneTransforms[MAX_MODEL_TRANSFORMS];
    uint BoneIndex;
};

cbuffer CB_AnimationFrame : register(b2)
{
    AnimationFrame Keyframes;
};

cbuffer CameraBuffer : register(b3)
{
    float3 CameraPosition;
    float padding;
};

/*
inout : �Ķ���� world�� ����¿����� ����ϰڴ�.
in : �Է¿�, out : ��¿�
*/
void SetAnimationWorld(out matrix animationWorld, in matrix world, VertexModel input)
{
    float indices[4] = { input.BlendIndices.x, input.BlendIndices.y, input.BlendIndices.z, input.BlendIndices.w };
    float weights[4] = { input.BlendWeights.x, input.BlendWeights.y, input.BlendWeights.z, input.BlendWeights.w };
    
    int clip;
    int currFrame;
    int nextFrame;
    float time;

    clip = Keyframes.Clip;
    currFrame = Keyframes.CurrFrame;
    nextFrame = Keyframes.NextFrame;
    time = Keyframes.Time;
    
    
    float4 c0, c1, c2, c3; // �� ������ �ȼ��� ����� �� �б�
    float4 n0, n1, n2, n3;
    
    matrix curr = 0;
    matrix transform = 0;

        [unroll(4)] //...? �Ƚᵵ �� ���� ���� ��.
    for (int i = 0; i < 4; i++)
    {
        // TransformsMap.Sample�� Ȯ��, ��� ������ �����Ͱ� ���ϹǷ� Load���.
        // Load�� �Ķ���ʹ� int4 w�� �Ӹʹ�ȣ �Ӹ��� ���常 ���� 0

        //                                    ��,        Ű������,   ��(Ŭ��)
        c0 = TransformsMap.Load(int4(indices[i] * 4 + 0, currFrame, clip, 0));
        c1 = TransformsMap.Load(int4(indices[i] * 4 + 1, currFrame, clip, 0));
        c2 = TransformsMap.Load(int4(indices[i] * 4 + 2, currFrame, clip, 0));
        c3 = TransformsMap.Load(int4(indices[i] * 4 + 3, currFrame, clip, 0));
        curr = matrix(c0, c1, c2, c3);
        
        transform += mul(weights[i], curr);
    }
    
    animationWorld = mul(transform, world);
}

VertexOutput main(VertexModel input)
{
    VertexOutput Out;
    Out.BlendIndices.x = input.BlendIndices.x;
    Out.BlendIndices.y = input.BlendIndices.y;
    Out.BlendIndices.z = input.BlendIndices.z;
    Out.BlendIndices.w = input.BlendIndices.w;

    Out.BlendWeights.x = input.BlendWeights.x;
    Out.BlendWeights.y = input.BlendWeights.y;
    Out.BlendWeights.z = input.BlendWeights.z;
    Out.BlendWeights.w = input.BlendWeights.w;

    // bone�� ��ġ�� �ű� �� ����� �Űܾ���
    //matrix boneMatrix;
    //boneMatrix = mul(BoneTransforms[BoneIndex], WorldMatrix);

    matrix animationWorld;
    SetAnimationWorld(animationWorld, WorldMatrix, input);

    Out.Tangent = mul(float4(input.Position, 1.f), animationWorld);

    Out.Position = mul(float4(input.Position, 1.f), animationWorld);
    Out.Position = mul(Out.Position, ViewMatrix);
    Out.Position = mul(Out.Position, ProjectionMatrix);

    Out.Uv = input.Uv;
    
    Out.Normal = normalize(mul(float4(input.Normal, 0.f), WorldMatrix));
    
    float4 WorldPosition = mul(float4(input.Position, 1.f), WorldMatrix);
    Out.WorldPos = WorldPosition;
    Out.ProjPos = Out.Position;
    //Out.ViewDirection = normalize(CameraPosition.xyz - WorldPosition.xyz);

    
    Out.Tangent = normalize(mul(float4(input.Tangent, 0.f), WorldMatrix));
    Out.BiTangent = normalize(mul(float4(input.BiTangent, 0.f), WorldMatrix));
    
  
    return Out;
}