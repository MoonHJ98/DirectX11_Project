// GLOBAL for Pixel Shader
Texture2D ShaderTexture : register(t0); // �޾ƿ� �ؽ��� 
Texture2D NormalTexture : register(t1); 
Texture2D SpecularTexture : register(t2);


SamplerState SampleType; // �ؽ��ĸ� ������ ���̵� �Ҷ� ���


struct PixelInput
{
    float4 Position : SV_POSITION;
    float2 Uv : TEXCOORD;
    float3 Normal : NORMAL;
    float3 Tangent : TANGENT;
    float3 BiTangent : BITANGENT;
    float4 BlendWeights : BLENDWEIGHTS;
    float4 BlendIndices : BLENDINDICES;
    //float3 ViewDirection : TEXCOORD1;
    vector WorldPos : TEXCOORD1;
    vector ProjPos : TEXCOORD2;
};
struct PixelOutput
{
    float4 Color : SV_TARGET0;
    float4 Normal : SV_TARGET1;
    float4 Specular : SV_TARGET2;
    float4 Depth : SV_TARGET3;
    float3 ViewDirection : SV_TARGET3;
};

//cbuffer LightBuffer : register(b0)
//{
//    float4 DiffuseColor;
//    float4 AmbientColor;
//    float4 SpecularColor;
//    float3 LightDirection;
//    float SpecularPower;  
//};

PixelOutput main(PixelInput input) : SV_TARGET
{
    float4 textureColor;
    float4 color;
    float3 reflection;
    float4 specular = float4(0.f, 0.f, 0.f, 0.f);
    float4 normalMap;
    float3 normal;
    float4 specularIntensity; // for Specular Mapping

    textureColor = ShaderTexture.Sample(SampleType, input.Uv);
    normalMap = NormalTexture.Sample(SampleType, input.Uv);
    
    // ���� ���� ������ 0~1���� -1 ~ 1�� Ȯ��. For Normal Mapping
    normalMap = (normalMap * 2.f) - 1.f;
    
    normal = (normalMap.x * input.Tangent) + (normalMap.y * input.BiTangent) + (normalMap.z * input.Normal);
    normal = normalize(normal);
    
    PixelOutput Out;

    Out.Color = textureColor;
    Out.Normal = float4(normal, 0.f);
    Out.Specular = SpecularTexture.Sample(SampleType, input.Uv);
    Out.Depth = vector(input.ProjPos.z / input.ProjPos.w, input.ProjPos.w / 1000.f, 0.f, 0.f);

    //Out.ViewDirection = input.ViewDirection;
    return Out;


    //// ��� �ȼ��� �⺻ ��� ������ Ambient�� ����. (For Directional + Ambient Light)
    //color = AmbientColor;
    //
    //
    //// ����� ���� �� ������ ���� ��Ŵ
    //float3 lightDir = -LightDirection;
    //
    //// ���� �� ���
    //float lightIntensity = saturate(dot(normal, lightDir));
    //
    //// for Directional + Ambient Light.
    //if(lightIntensity > 0.f)
    //{
    //    color += saturate(DiffuseColor * lightIntensity);
    //    
    //    // for Specular
    //    {
    //        specularIntensity = SpecularTexture.Sample(SampleType, input.Uv); // For Specular Mapping
    //
    //        reflection = normalize(2 * lightIntensity * normal - lightDir);
    //        specular = SpecularColor * pow(saturate(dot(reflection, input.ViewDirection)), SpecularPower);
    //
    //        specular = specular * specularIntensity; // For Specular Mapping
    //
    //    }
    //}
    //// ���� ���� ����(for Directinal Light)
    ////color = saturate(DiffuseColor * lightIntensity);
    //
    //// �ؽ��Ŀ� ���� ���Ͽ� ���� �ȼ� ���� ��� ���.
    //color = color * textureColor;
    //
    //// for Specular
    //color = saturate(color + specular);
    //return color;
}

/*
Specular Light(�ݻ籤)
�ݻ籤 = �ݻ���� �� * (���¹���� �ݻ籤�� ����)^�ݻ簭��

�ݻ纤��(�ݻ籤) = 2 * ���� ���� * ���� - ���� ����

���¹��� : ī�޶� ��ġ - ������ġ

*/