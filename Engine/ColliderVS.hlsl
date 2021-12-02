
cbuffer MatrixBuffer : register(b0)
{
    matrix worldMatrix;
    matrix viewMatrix;
    matrix projectionMatrix;
    matrix lightViewMatrix;
    matrix lightProjectionMatrix;
    bool renderDepthForShadow;
    bool padding[3];
};


struct VertexInputType
{
    float3 position : POSITION;
    float3 color : COLOR;
    float2 uv : TEXCOORD0;
};

struct PixelInputType
{
    float4 position : SV_POSITION0;
    float2 uv : TEXCOORD0;
    float4 normal : NORMAL;
	float4 color : COLOR;
    float4 WorldPos : TEXCOORD1;
    float4 ProjPos : TEXCOORD2;
    float4 lightViewPosition : TEXCOORD3;
    bool renderDepthForShadow : BOOL;
};

PixelInputType main(VertexInputType input)
{
    PixelInputType output;
    
    output.position = mul(float4(input.position, 1.f), worldMatrix);
    output.position = mul(output.position, viewMatrix);
    output.position = mul(output.position, projectionMatrix);
    
    output.uv = input.uv;
	output.color = float4(input.color, 1.f);
   
    
    return output;
}