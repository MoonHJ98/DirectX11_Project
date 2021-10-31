// GLOBAL for Pixel Shader
Texture2D ShaderTexture : register(t0); // 받아온 텍스쳐 
Texture2D NormalTexture : register(t1); 
Texture2D SpecularTexture : register(t2);


SamplerState SampleType; // 텍스쳐를 도형에 셰이딩 할때 사용


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
    
    // 정상 값의 범위를 0~1에서 -1 ~ 1로 확장. For Normal Mapping
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


    //// 모든 픽셀의 기본 출력 색상을 Ambient로 설정. (For Directional + Ambient Light)
    //color = AmbientColor;
    //
    //
    //// 계산을 위해 빛 방향을 반전 시킴
    //float3 lightDir = -LightDirection;
    //
    //// 빛의 양 계산
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
    //// 최종 색생 결정(for Directinal Light)
    ////color = saturate(DiffuseColor * lightIntensity);
    //
    //// 텍스쳐와 빛을 곱하여 최종 픽셀 색상 결과 얻기.
    //color = color * textureColor;
    //
    //// for Specular
    //color = saturate(color + specular);
    //return color;
}

/*
Specular Light(반사광)
반사광 = 반사빛의 색 * (보는방향과 반사광의 내적)^반사강도

반사벡터(반사광) = 2 * 빛의 강도 * 법선 - 빛의 방향

보는방향 : 카메라 위치 - 정점위치

*/