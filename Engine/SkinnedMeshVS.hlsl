cbuffer MatrixBuffer : register(b0)
{
    matrix WorldMatrix;
    matrix ViewMatrix;
    matrix ProjectionMatrix;

}
struct SkinnedInput
{
    float3 Position : POSITION0;   
    float2 Tex : TEXCOORD0;          
    float3 Normal : NORMAL0;
    float3 Weights : WEIGHT0;
    uint4 BoneIndices : BLENDINDICES;  
};

struct SkinnedOutput
{
    float4 Position : SV_POSITION;
    float2 Tex : TEXCOORD;
    float3 Normal : NORMAL;
    float3 Weights : WEIGHT0;
    uint4 BoneIndices : BLENDINDICES;

};

SkinnedOutput main(SkinnedInput _Input)
{
    SkinnedOutput Out;
    
    float weight[4];
    //weight[0] = _Input.Weights.x;
    //weight[1] = _Input.Weights.y;
    //weight[2] = _Input.Weights.z;
    //weight[3] = _Input.Weights.w;

    Out.Position = mul(float4(_Input.Position, 1.f), WorldMatrix);
    Out.Weights = _Input.Weights;

    Out.Position = mul(Out.Position, ViewMatrix);
    Out.Position = mul(Out.Position, ProjectionMatrix);

    Out.Tex = _Input.Tex;
    
    Out.Normal = normalize(mul(float4(_Input.Normal, 0.f), WorldMatrix));

    
    return Out;

}