Texture2D DiffuseTexture : register(t0);
SamplerState SampleType;


struct PixelInputType
{
    float4 position : SV_POSITION;
    float2 uv : TEXCOORD0;
    float4 normal : NORMAL;
    float4 WorldPos : TEXCOORD1;
    float4 ProjPos : TEXCOORD2;
    float4 depethforLight : TEXCOORD3;
    float4 lightViewPosition : TEXCOORD4;

    
};

struct Output
{
    float4 color : SV_TARGET0;
    float4 normal : SV_TARGET1;
    float4 Specular : SV_TARGET2;
    float4 depth : SV_TARGET3;
    
};

Output main(PixelInputType input)
{
    Output Out;
    Out.color = DiffuseTexture.Sample(SampleType, input.uv);
    Out.normal = input.normal;

    
    Out.depth = float4(input.ProjPos.z / input.ProjPos.w, input.ProjPos.w / 1000.f, 0.f, 0.f);
    Out.Specular = float4(0.f, 0.f, 0.f, 0.f);
    float depthValue = input.depethforLight.z / input.depethforLight.w;
    
    return Out;
}