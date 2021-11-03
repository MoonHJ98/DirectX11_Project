#define MAX_MODEL_TRANSFORMS 250
#define MAX_MODEL_KEYFRAMES 500
Texture2DArray TransformsMap : register(t0);


struct VertexModel
{
    float3 Position : POSITION;
    float2 Uv : TEXCOORD;
    float3 Normal : NORMAL;
    float3 Tangent : TANGENT;
    float3 BiTangent : BITANGENT;
    float4 BlendWeights : BLENDWEIGHTS;
    float4 BlendIndices : BLENDINDICES;

};

struct VertexOutput
{
    float4 Position : SV_POSITION;
    float2 Uv : TEXCOORD;
    float3 Normal : NORMAL;
    float3 Tangent : TANGENT;
    float3 BiTangent : BITANGENT;
    float4 BlendWeights : BLENDWEIGHTS;
    float4 BlendIndices : BLENDINDICES;  

    float4 WorldPos : TEXCOORD1;
    float4 ProjPos : TEXCOORD2;

};

struct AnimationFrame
{
    int Clip;

    uint CurrFrame;
    uint NextFrame;

    float Time;
    float Running;

    float3 Padding;
};


cbuffer MatrixBuffer : register(b0)
{
    matrix WorldMatrix;
    matrix ViewMatrix;
    matrix ProjectionMatrix;
}

cbuffer CB_Bone : register(b1)
{
    matrix BoneTransforms[MAX_MODEL_TRANSFORMS];
    uint BoneIndex;
};

cbuffer CB_AnimationFrame : register(b2)
{
    AnimationFrame Keyframes;
};

cbuffer CameraBuffer : register(b3)
{
    float3 CameraPosition;
    float padding;
};

/*
inout : 파라미터 world를 입출력용으로 사용하겠다.
in : 입력용, out : 출력용
*/
void SetAnimationWorld(out matrix animationWorld, in matrix world, VertexModel input)
{
    float indices[4] = { input.BlendIndices.x, input.BlendIndices.y, input.BlendIndices.z, input.BlendIndices.w };
    float weights[4] = { input.BlendWeights.x, input.BlendWeights.y, input.BlendWeights.z, input.BlendWeights.w };
    
    int clip;
    int currFrame;
    int nextFrame;
    float time;

    clip = Keyframes.Clip;
    currFrame = Keyframes.CurrFrame;
    nextFrame = Keyframes.NextFrame;
    time = Keyframes.Time;
    
    
    float4 c0, c1, c2, c3; // 행 단위로 픽셀에 써놓은 것 읽기
    float4 n0, n1, n2, n3;
    
    matrix curr = 0;
    matrix transform = 0;

        [unroll(4)] //...? 안써도 됨 아직 뭔지 모름.
    for (int i = 0; i < 4; i++)
    {
        // TransformsMap.Sample은 확대, 축소 때문에 데이터가 변하므로 Load사용.
        // Load의 파라미터는 int4 w는 밉맵번호 밉맵을 한장만 쓰니 0

        //                                    본,        키프레임,   면(클립)
        c0 = TransformsMap.Load(int4(indices[i] * 4 + 0, currFrame, clip, 0));
        c1 = TransformsMap.Load(int4(indices[i] * 4 + 1, currFrame, clip, 0));
        c2 = TransformsMap.Load(int4(indices[i] * 4 + 2, currFrame, clip, 0));
        c3 = TransformsMap.Load(int4(indices[i] * 4 + 3, currFrame, clip, 0));
        curr = matrix(c0, c1, c2, c3);
        
        transform += mul(weights[i], curr);
    }
    
    animationWorld = mul(transform, world);
}

VertexOutput main(VertexModel input)
{
    VertexOutput Out;
    Out.BlendIndices.x = input.BlendIndices.x;
    Out.BlendIndices.y = input.BlendIndices.y;
    Out.BlendIndices.z = input.BlendIndices.z;
    Out.BlendIndices.w = input.BlendIndices.w;

    Out.BlendWeights.x = input.BlendWeights.x;
    Out.BlendWeights.y = input.BlendWeights.y;
    Out.BlendWeights.z = input.BlendWeights.z;
    Out.BlendWeights.w = input.BlendWeights.w;

    // bone의 위치를 옮긴 후 월드로 옮겨야함
    //matrix boneMatrix;
    //boneMatrix = mul(BoneTransforms[BoneIndex], WorldMatrix);

    matrix animationWorld;
    SetAnimationWorld(animationWorld, WorldMatrix, input);

    Out.Tangent = mul(float4(input.Position, 1.f), animationWorld);

    Out.Position = mul(float4(input.Position, 1.f), animationWorld);
    Out.Position = mul(Out.Position, ViewMatrix);
    Out.Position = mul(Out.Position, ProjectionMatrix);

    Out.Uv = input.Uv;
    
    Out.Normal = normalize(mul(float4(input.Normal, 0.f), WorldMatrix));
    
    float4 WorldPosition = mul(float4(input.Position, 1.f), WorldMatrix);
    Out.WorldPos = WorldPosition;
    Out.ProjPos = Out.Position;
    //Out.ViewDirection = normalize(CameraPosition.xyz - WorldPosition.xyz);

    
    Out.Tangent = normalize(mul(float4(input.Tangent, 0.f), WorldMatrix));
    Out.BiTangent = normalize(mul(float4(input.BiTangent, 0.f), WorldMatrix));
    
  
    return Out;
}