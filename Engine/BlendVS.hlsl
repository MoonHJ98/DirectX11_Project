
cbuffer MatrixBuffer : register(b0)
{
    matrix WorldMatrix;
    matrix ViewMatrix;
    matrix ProjectionMatrix;
}

struct VSInput
{
    float3 Position : POSITION;
    float2 Uv : TEXCOORD;
};

struct VSOutput
{
    float4 Position : SV_POSITION;
    float2 Uv : TEXCOORD;
};


VSOutput main(VSInput input)
{
    VSOutput Out;


    Out.Position = mul(float4(input.Position, 1.f), WorldMatrix);
    Out.Position = mul(Out.Position, ViewMatrix);
    Out.Position = mul(Out.Position, ProjectionMatrix);

    Out.Uv = input.Uv;

    return Out;
}