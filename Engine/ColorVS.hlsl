
cbuffer MatrixBuffer
{
    matrix worldMatrix;
    matrix viewMatrix;
    matrix projectionMatrix;
};


struct VertexInputType
{
    float3 position : POSITION;
    float4 color : COLOR;
};

struct PixelInputType
{
    float4 position : SV_POSITION;
    float4 color : COLOR;
    float4 WorldPos : TEXCOORD0;
    float4 ProjPos : TEXCOORD1;
};


PixelInputType main(VertexInputType input)
{
    PixelInputType output;
    
    output.position = mul(float4(input.position, 1.f), worldMatrix);
    output.position = mul(output.position, viewMatrix);
    output.position = mul(output.position, projectionMatrix);
    
    output.color = input.color;
    
    float4 WorldPosition = mul(float4(input.position, 1.f), worldMatrix);
    output.WorldPos = WorldPosition;
    output.ProjPos = output.position;
    
    
    return output;
}