
Texture2D shaderTexture : register(t0);
SamplerState SampleType;


struct PixelInputType
{
    float4 position : SV_POSITION;
    float2 tex : TEXCOORD;
};

float4 main(PixelInputType input) : SV_TARGET
{
    return shaderTexture.Sample(SampleType, input.tex);
}