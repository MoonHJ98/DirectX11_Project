Texture2D NormalTexture : register(t0);
Texture2D SpecularTexture : register(t1);
Texture2D DepthTexture : register(t2);
Texture2D ShadowTexture : register(t3);
Texture2D LightViewPosTexture : register(t4);







SamplerState SampleType; // �ؽ��ĸ� ������ ���̵� �Ҷ� ���

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
    float lightIntensity;

    float bias = 0.001f;
    float2 projectTexCoord;
    float depthValue;
    float lightDepthValue;
    float4 lightViewPosition;

    


    float4 vDepthInfo = DepthTexture.Sample(SampleType, input.Uv);


    if (vDepthInfo.b != 1.f)
        color = AmbientColor;
    
    
    lightDir = -LightDirection;

    normal = NormalTexture.Sample(SampleType, input.Uv);
    specularIntensity = SpecularTexture.Sample(SampleType, input.Uv);

    lightViewPosition = LightViewPosTexture.Sample(SampleType, input.Uv);
    projectTexCoord.x = lightViewPosition.x / lightViewPosition.w / 2.0f + 0.5f;
    projectTexCoord.y = -lightViewPosition.y / lightViewPosition.w / 2.0f + 0.5f;

    if ((saturate(projectTexCoord.x) == projectTexCoord.x) && (saturate(projectTexCoord.y) == projectTexCoord.y))
    {
            // ���� �� �ؽ�ó ��ǥ ��ġ���� ���÷��� ����Ͽ� ���� �ؽ�ó���� ������ �� ���� ���� ���ø��մϴ�.
        depthValue = ShadowTexture.Sample(SampleType, projectTexCoord).r;
		
		    // ���� ���̸� ����մϴ�.
        lightDepthValue = lightViewPosition.z / lightViewPosition.w;
		
		    // lightDepthValue���� ���̾�� ���ϴ�.
        lightDepthValue = lightDepthValue - bias;
		
		    // ������ �� ���� ���̿� ���� ���̸� ���Ͽ��� �ȼ��� ���� ó������ �������� �����մϴ�.
		    // ���� ��ü �տ� ������ �ȼ��� ���߰�, �׷��� ������ ��ü (��Ŭ ���)�� �׸��ڸ� �帮 ��� �������� �ȼ��� �׸��ڷ� �׸��ϴ�.
        if (lightDepthValue < depthValue)
        {
			    // �� �ȼ��� ���� ���� ����մϴ�.
            lightIntensity = saturate(dot(normal, lightDir));
			
            if (lightIntensity > 0.0f)
            {
				    // Ȯ�� ���� �� ������ �翡 ���� ���� Ȯ����� �����մϴ�.
                color += saturate(DiffuseColor * lightIntensity);

                // for Specular
                float fViewZ = vDepthInfo.y * 1000.f; // 1000 : far
                
                float4 vPosition;
                
	            // ������ı��� ���س��� ��ġ.
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
    else
	{
        
		lightIntensity = saturate(dot(normal, lightDir));
		
		if (lightIntensity > 0.f)
		{
			color += saturate(DiffuseColor * lightIntensity);
		}
		if (normal.x == 0.f && normal.y == 0.f && normal.z == 0.f)
		{
			color = DiffuseColor;
		}

	}


    Out.shade = color;
    Out.specular = specular;
    return Out;
}
