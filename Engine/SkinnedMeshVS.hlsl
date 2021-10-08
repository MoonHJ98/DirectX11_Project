cbuffer MatrixBuffer : register(b0)
{
    matrix WorldMatrix;
    matrix ViewMatrix;
    matrix ProjectionMatrix;
}

cbuffer cbSkinned : register(b1)
{
    float4x4 BoneTransforms[96];
};



struct SkinnedInput
{
    float3 Position : POSITION0;   
    float2 Tex : TEXCOORD0;          
    float3 Normal : NORMAL0;
    float4 Weights : WEIGHT0;
    uint4 BoneIndices : BLENDINDICES;  
};


struct SkinnedOutput
{
    float4 Position : SV_POSITION;
    float2 Tex : TEXCOORD;
    float3 Normal : NORMAL;
    float Weights[4] : WEIGHT0;
    uint4 BoneIndices : BLENDINDICES;
    float3 PositionLocal : POSITION;


};

SkinnedOutput main(SkinnedInput _Input)
{
    SkinnedOutput Out;

    Out.BoneIndices = _Input.BoneIndices;

	float3 posL     = float3(0.0f, 0.0f, 0.0f);
    
    Out.Weights[0] = _Input.Weights.x;
    Out.Weights[1] = _Input.Weights.y;
    Out.Weights[2] = _Input.Weights.z;
    Out.Weights[3] = _Input.Weights.w;

    for (int i = 0; i < 4; ++i)
    {
        Out.PositionLocal += Out.Weights[i] * mul(float4(_Input.Position, 1.0f), BoneTransforms[0]).xyz;
    }
    BoneTransforms[0].rgb;


    Out.Position = mul(float4(_Input.Position, 1.f), WorldMatrix);
    Out.Position = mul(Out.Position, ViewMatrix);
    Out.Position = mul(Out.Position, ProjectionMatrix);
    
    Out.Tex = _Input.Tex;
    
    Out.Normal = normalize(mul(float4(_Input.Normal, 0.f), WorldMatrix));

    
    return Out;

}