
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
    float3 normal : NORMAL;
    float2 uv : TEXCOORD0;
    float4 color : COLOR;
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
    float4 WorldPosition = output.position;
    output.position = mul(output.position, viewMatrix);
    output.position = mul(output.position, projectionMatrix);
    
    output.uv = input.uv;
    output.normal = normalize(mul(float4(input.normal, 0.f), worldMatrix));
    

    output.WorldPos = WorldPosition;
    output.ProjPos = output.position;
   
    output.renderDepthForShadow = renderDepthForShadow;
    
    output.lightViewPosition = mul(WorldPosition, lightViewMatrix);
    output.lightViewPosition = mul(output.lightViewPosition, lightProjectionMatrix);
    
    return output;
}