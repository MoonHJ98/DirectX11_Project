// GLOBAL for Pixel Shader
Texture2D ShaderTexture; // �޾ƿ� �ؽ���
SamplerState SampleType; // �ؽ��ĸ� ������ ���̵� �Ҷ� ���

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