
Texture2D shaderTexture : register(t0);
SamplerState SampleType;


struct PixelInputType
{
    float4 position : SV_POSITION;
    float2 tex : TEXCOORD;
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
    
    Out.color = shaderTexture.Sample(SampleType, input.tex);
    Out.color.a = 1.f;
    Out.normal = float4(0.f, 0.f, 0.f, 0.f);
    Out.depth = float4(0.f, 0.f, 0.f, 0.f);
    Out.Specular = float4(0.f, 0.f, 0.f, 0.f);
    Out.lightViewPosition = float4(0.f, 0.f, 0.f, 0.f);
   
    return Out;
}