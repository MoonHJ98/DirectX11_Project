// GLOBAL for Pixel Shader
Texture2D ShaderTexture; // 받아온 텍스쳐
SamplerState SampleType; // 텍스쳐를 도형에 셰이딩 할때 사용

struct VertexOutput
{
    float4 Position : SV_POSITION;
    float2 Uv : TEXCOORD;
    float3 Normal : NORMAL;

};

float4 main(VertexOutput input) : SV_TARGET
{
    float4 textureColor;
    
    textureColor = ShaderTexture.Sample(SampleType, input.Uv);
    
    return textureColor;
}