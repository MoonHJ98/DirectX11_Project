Texture2D BasicTexture : register(t0);
Texture2D alphaMap0 : register(t1);
Texture2D DiffuseTexture0 : register(t2);
Texture2D DiffuseTexture1 : register(t3);
Texture2D DiffuseTexture2 : register(t4);



SamplerState SampleType;

cbuffer BrushDesc : register(b0)
{
    float4 terrainBrushColor;
    float3 terrainBrushPosition;
    uint terrainBrushRange;

    uint smoothSize;

    uint terrainHeight;
    uint terrainWidth;
    uint padding4;
};

cbuffer BrushDesc : register(b1)
{
    bool diffuse;
    bool normal;
    bool specular;
    bool padding;
};

cbuffer toolDesc : register(b2)
{
    bool terrainTool;
    bool padding2[3];
};

cbuffer TextureInfoBuffer : register(b3)
{
    bool useAlpha;
    bool padding3[3];
}

struct PixelInputType
{
    float4 position : SV_POSITION;
    float2 uv : TEXCOORD0;
    float4 normal : NORMAL;
    float4 color : COLOR;
    float4 WorldPos : TEXCOORD1;
    float4 ProjPos : TEXCOORD2;
    float4 lightViewPosition : TEXCOORD3;
    bool renderDepthForShadow : BOOL;

  
};

struct Output
{
    float4 color : SV_TARGET0;
    float4 normal : SV_TARGET1;
    float4 Specular : SV_TARGET2;
    float4 depth : SV_TARGET3;
    float4 lightViewPosition : SV_TARGET4;
    
};

float4 GetBrushColor(float4 inputWorldPos, float2 inputUV)
{
    
    float dx = inputWorldPos.x - terrainBrushPosition.x;
    float dz = inputWorldPos.z - terrainBrushPosition.z;
    
    float dist = sqrt(dx * dx + dz * dz);
    
    if (dist <= (float) terrainBrushRange)
    {
        //float4 color = HeightTexture.Sample(SampleType, inputUV);
        //return color;
        return terrainBrushColor;
    }
    
    return float4(0.f, 0.f, 0.f, 1.f);
}

Output main(PixelInputType input)
{
    Output Out;
    Out.color = float4(1.f, 1.f, 1.f, 1.f);
    
    float4 basicTexture;
    float4 texture0 = float4(0.f, 0.f, 0.f, 0.f);
    float4 texture1 = float4(0.f, 0.f, 0.f, 0.f);
    float4 texture2 = float4(0.f, 0.f, 0.f, 0.f);


    float4 color;
    float4 alphaValue0;
    float4 alphaValue1;

    float4 blendColor;

    if (diffuse)
        basicTexture = BasicTexture.Sample(SampleType, input.uv);

    //if(diffuse)
    //    Out.color = DiffuseTexture.Sample(SampleType, input.uv);


    
    Out.normal = input.normal;

    
    Out.depth = float4(input.ProjPos.z / input.ProjPos.w, input.ProjPos.w / 1000.f, 0.f, 0.f);
    Out.Specular = float4(0.f, 0.f, 0.f, 0.f);
    Out.lightViewPosition = input.lightViewPosition;

    if (useAlpha)
    {
        // 두 번째 책스쳐의 픽셀 색상
        texture0 = DiffuseTexture0.Sample(SampleType, input.uv);
        texture1 = DiffuseTexture1.Sample(SampleType, input.uv);
        texture2 = DiffuseTexture2.Sample(SampleType, input.uv);



        // 알파 블렌딩 값을 샘플링한다.

        alphaValue0 = alphaMap0.Sample(SampleType, input.uv); 
        //texture0 = alphaValue0.r * texture0;

       
        //alphaValue1 = alphaMap1.Sample(SampleType, input.uv);
       // texture1 = alphaValue0.g * texture1;
     
        // 알파는 알파값에 따라 두 색상을 혼합한다.
        //blendColor = ((1.0 - alphaValue0) * basicTexture);
        

        blendColor = lerp(basicTexture, texture0, alphaValue0.r);
        blendColor = lerp(blendColor, texture1, alphaValue0.g);
        blendColor = lerp(blendColor, texture2, alphaValue0.b);



        //blendColor = texture0 + ((1.0 - alphaValue0) * basicTexture);
    }
    else
    {
        blendColor = BasicTexture.Sample(SampleType, input.uv);
    }


    if (input.renderDepthForShadow == true)
    {
        float4 outputcolor = float4(0.f, 0.f, 0.f, 1.f);
        float depthValue = input.ProjPos.z / input.ProjPos.w;

        outputcolor = float4(depthValue, depthValue, depthValue, 1.0f);
        Out.color = outputcolor;
        Out.normal = outputcolor;
        Out.Specular = outputcolor;
        Out.depth = outputcolor;
        Out.lightViewPosition = outputcolor;

        return Out;
    }
    
    color = blendColor;

    if (terrainTool)
        color += GetBrushColor(input.WorldPos, input.uv);

    Out.color = color;
    return Out;
}