
Texture2D shaderTexture : register(t0);
SamplerState SampleType;


struct PixelInputType
{
    float4 position : SV_POSITION;
    float2 tex : TEXCOORD;
};

float4 main(PixelInputType input) : SV_TARGET
{
    return float4(1.f, 1.f, 1.f, 0.f) /*shaderTexture.Sample(SampleType, input.tex)*/;
}