
Texture2D shaderTexture : register(t0);
Texture2D depthTexture : register(t1);

SamplerState SampleType;

cbuffer ProjtoWorld : register(b0)
{
    matrix ViewInv;
    matrix ProjInv;
    float3 CamPos;
    float padding;
};

struct PixelInputType
{
    float4 position : SV_POSITION;
    float2 tex : TEXCOORD;
    float3 color : COLOR;
    float4 ProjPos : TEXCOORD1;
    float4 WorldPos : TEXCOORD2;


};

struct Output
{
    float4 color : SV_TARGET0;
    float4 normal : SV_TARGET1;
    float4 Specular : SV_TARGET2;
    float4 depth : SV_TARGET3;
    float4 lightViewPosition : SV_TARGET4;
    
};

Output main(PixelInputType input) : SV_TARGET
{
    Output Out;
   
    
    float4 textureColor = shaderTexture.Sample(SampleType, input.tex);

    Out.color = textureColor;


    Out.normal = float4(0.f, 0.f, 0.f, 0.f);
    Out.depth = float4(0.f, 0.f, 0.f, 0.f);

    Out.Specular = float4(0.f, 0.f, 0.f, 0.f);
    Out.lightViewPosition = float4(0.f, 0.f, 0.f, 0.f);
   
    return Out;
}