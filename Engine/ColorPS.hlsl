Texture2D DiffuseTexture : register(t0);
Texture2D HeightTexture : register(t1);

SamplerState SampleType;

cbuffer BrushDesc : register(b1)
{
    bool diffuse;
    bool normal;
    bool specular;
    bool padding;
};

struct PixelInputType
{
    float4 position : SV_POSITION;
    float2 uv : TEXCOORD0;
    float4 normal : NORMAL;
    float4 WorldPos : TEXCOORD1;
    float4 ProjPos : TEXCOORD2;
    float4 lightViewPosition : TEXCOORD3;
    bool renderDepthForShadow : BOOL;

  
};

struct Output
{
    float4 color : SV_TARGET0;
    float4 normal : SV_TARGET1;
    float4 Specular : SV_TARGET2;
    float4 depth : SV_TARGET3;
    float4 lightViewPosition : SV_TARGET4;
    
};

Output main(PixelInputType input)
{
    Output Out;
    Out.color = float4(1.f, 1.f, 1.f, 1.f);
    
    if(diffuse)
        Out.color = DiffuseTexture.Sample(SampleType, input.uv);

    
    Out.normal = input.normal;

    
    Out.depth = float4(input.ProjPos.z / input.ProjPos.w, input.ProjPos.w / 1000.f, 0.f, 0.f);
    Out.Specular = float4(0.f, 0.f, 0.f, 0.f);
    Out.lightViewPosition = input.lightViewPosition;

    if (input.renderDepthForShadow == true)
    {
        float4 color = float4(0.f, 0.f, 0.f, 1.f);
        float depthValue = input.ProjPos.z / input.ProjPos.w;

        color = float4(depthValue, depthValue, depthValue, 1.0f);
        Out.color = color;
        Out.normal = color;
        Out.Specular = color;
        Out.depth = color;
        Out.lightViewPosition = color;

    }
    
    return Out;
}