
cbuffer MatrixBuffer
{
    matrix worldMatrix;
    matrix viewMatrix;
    matrix projectionMatrix;
};


struct VertexInputType
{
    float3 position : POSITION;
    float2 uv : TEXCOORD0;
    float3 normal : NORMAL;
};

struct PixelInputType
{
    float4 position : SV_POSITION;
    float2 uv : TEXCOORD0;
    float4 normal : NORMAL;
    float4 WorldPos : TEXCOORD1;
    float4 ProjPos : TEXCOORD2;
};


PixelInputType main(VertexInputType input)
{
    PixelInputType output;
    
    output.position = mul(float4(input.position, 1.f), worldMatrix);
    output.position = mul(output.position, viewMatrix);
    output.position = mul(output.position, projectionMatrix);
    
    output.uv = input.uv;
    output.normal = normalize(mul(float4(input.normal, 0.f), worldMatrix));
    
    float4 WorldPosition = mul(float4(input.position, 1.f), worldMatrix);
    output.WorldPos = WorldPosition;
    output.ProjPos = output.position;
    
    
    return output;
}