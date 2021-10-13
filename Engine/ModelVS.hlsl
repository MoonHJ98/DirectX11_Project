#define MAX_MODEL_TRANSFORMS 250

struct VertexModel
{
    float3 Position : POSITION;
    float2 Uv : TEXCOORD;
    float3 Normal : NORMAL;
    float3 Tangent : TANGENT;
    float4 BlendWeights : BLENDWEIGHTS;
    float4 BlendIndices : BLENDINDICES;

};

struct VertexOutput
{
    float4 Position : SV_POSITION;
    float2 Uv : TEXCOORD;
    float3 Normal : NORMAL;

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

VertexOutput main(VertexModel input)
{
    VertexOutput Out;

    // bone의 위치를 옮긴 후 월드로 옮겨야함
    matrix boneMatrix;
    boneMatrix = mul(BoneTransforms[BoneIndex], WorldMatrix);

    Out.Position = mul(float4(input.Position, 1.f), boneMatrix);
    Out.Position = mul(Out.Position, ViewMatrix);
    Out.Position = mul(Out.Position, ProjectionMatrix);

    Out.Uv = input.Uv;
    
    Out.Normal = normalize(mul(float4(input.Normal, 0.f), WorldMatrix));

    return Out;
}