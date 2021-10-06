// GLOBAL for Pixel Shader
Texture2D ShaderTexture; // 받아온 텍스쳐
SamplerState SampleType; // 텍스쳐를 도형에 셰이딩 할때 사용

struct SkinnedOutput
{
    float4 Position : SV_POSITION;
    float2 Tex : TEXCOORD0;
    //float3 Normal : NORMAL;
    float4 Weights : BLENDWEIGHT;
    //uint4 BoneIndices : BLENDINDICES;
};


float4 main(SkinnedOutput _Input) : SV_TARGET
{
    float4 textureColor;
    
    textureColor = ShaderTexture.Sample(SampleType, _Input.Tex);
    
    return textureColor;
}