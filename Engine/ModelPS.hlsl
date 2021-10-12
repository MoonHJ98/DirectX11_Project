float3 Directrion = float3(-1.f, -1.f, 1.f);

struct VertexOutput
{
    float4 Position : SV_POSITION;
    float2 Uv : TEXCOORD;
    float3 Normal : NORMAL;

};

float4 main(VertexOutput input)
{
    float NDotL = dot(normalize(input.Normal), -Directrion);

    return float4(1.f, 1.f, 1.f, 1.f) * NDotL;
}