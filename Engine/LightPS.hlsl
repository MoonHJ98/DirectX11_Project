Texture2D NormalTexture : register(t0);
Texture2D SpecularTexture : register(t1);
Texture2D DepthTexture : register(t2);





SamplerState SampleType; // 텍스쳐를 도형에 셰이딩 할때 사용

cbuffer LightBuffer : register(b0)
{
    float4 DiffuseColor;
    float4 AmbientColor;
    float4 SpecularColor;
    float3 LightDirection;
    float SpecularPower;
};

cbuffer ProjtoWorld : register(b1)
{
    matrix ViewInv;
    matrix ProjInv;
    float3 CamPos;
    float padding;
};

cbuffer LightMatrixBuffer : register(b2)
{
    matrix lightViewMatrix;
    matrix lightProjMatrix;
    float3 lightPosition;
    float padding2;
}


struct PSInput
{
    float4 Position : SV_POSITION;
    float2 Uv : TEXCOORD;
};

struct PSOut
{
    float4 shade : SV_TARGET0;
    float4 specular : SV_TARGET1;
    float4 shadow : SV_TARGET2;
};

PSOut main(PSInput input)
{
    PSOut Out;

    float4 color = float4(0.f, 0.f, 1.f, 1.f);
    float3 lightDir;
    float3 normal;
    float4 specularIntensity;
    float3 viewDirection;
    float4 specular = float4(0.f, 0.f, 0.f, 0.f);
    float3 reflection;


    normal = NormalTexture.Sample(SampleType, input.Uv);
    specularIntensity = SpecularTexture.Sample(SampleType, input.Uv);
    float4 vDepthInfo = DepthTexture.Sample(SampleType, input.Uv);

    if (vDepthInfo.b != 1.f)
        color = AmbientColor;
    
    
    lightDir = -LightDirection;
    float lightIntensity = saturate(dot(normal, lightDir));

    
    if (lightIntensity > 0.f)
    {
        color += saturate(DiffuseColor * lightIntensity);

        // for Specular
        {
            if (!(vDepthInfo.r == 0.f && vDepthInfo.g == 0.f && vDepthInfo.b == 1.f))
            {
            
                float fViewZ = vDepthInfo.y * 1000.f; // 1000 : far

                float4 vPosition;

	            // 투영행렬까지 곱해놓은 위치.
                vPosition.x = ((input.Uv.x * 2.f - 1.f)) * fViewZ;
                vPosition.y = ((input.Uv.y * -2.f + 1.f)) * fViewZ;
                vPosition.z = vDepthInfo.x * fViewZ;
                vPosition.w = 1.f * fViewZ;

                vPosition = mul(vPosition, ProjInv);
                vPosition = mul(vPosition, ViewInv);

                float3 vLook = vPosition - float4(CamPos, 1.f);

                reflection = normalize(2 * lightIntensity * normal - lightDir);
                specular = SpecularColor * pow(saturate(dot(reflection, vLook)), SpecularPower);
    
                specular = specular * specularIntensity; // For Specular Mapping
            }
    
        }
       
    }


    
    Out.shade = color;
    Out.specular = specular;
    return Out;
}
