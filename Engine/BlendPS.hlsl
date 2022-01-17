Texture2D DiffuseTexture : register(t0);
Texture2D ShadeTexture : register(t1);
Texture2D SpecularTexture : register(t2);


SamplerState SampleType; // 텍스쳐를 도형에 셰이딩 할때 사용


struct PSInput
{
    float4 Position : SV_POSITION;
    float2 Uv : TEXCOORD;
};

float4 main(PSInput input) : SV_TARGET
{
    float4 diffuse;
    float4 shade;
    float4 color;
    float4 specular;
    diffuse = DiffuseTexture.Sample(SampleType, input.Uv);
    shade = ShadeTexture.Sample(SampleType, input.Uv);
    specular = SpecularTexture.Sample(SampleType, input.Uv);


    if(diffuse.a != 0)
    {
        color = shade * diffuse;
        color = saturate(color + specular);
    }
    else
        color = diffuse;

    return color;
}