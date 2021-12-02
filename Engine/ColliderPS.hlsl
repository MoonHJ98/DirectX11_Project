Texture2D DiffuseTexture : register(t0);
SamplerState SampleType;

cbuffer BrushDesc : register(b0)
{
    float4 terrainBrushColor;
    float3 terrainBrushPosition;
    uint terrainBrushRange;
};

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
	float4 color : COLOR;
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

float4 GetBrushColor(float4 inputWorldPos)
{
    
    float dx = inputWorldPos.x - terrainBrushPosition.x;
    float dz = inputWorldPos.z - terrainBrushPosition.z;
    
    float dist = sqrt(dx * dx + dz * dz);
    
    if (dist <= (float) terrainBrushRange)
        return terrainBrushColor;
    
    return float4(0.f, 0.f, 0.f, 1.f);
}

Output main(PixelInputType input)
{
    Output Out;
    Out.color = input.color;
    
	Out.normal = float4(0.f, 0.f, 0.f, 0.f);
    Out.depth = float4(0.f, 0.f, 0.f, 0.f);
    Out.Specular = float4(0.f, 0.f, 0.f, 0.f);
	Out.lightViewPosition = float4(0.f, 0.f, 0.f, 0.f);
    
    return Out;
}