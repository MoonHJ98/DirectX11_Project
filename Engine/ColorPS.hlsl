struct PixelInputType
{
    float4 position : SV_POSITION;
    float4 color : COLOR;
    float4 WorldPos : TEXCOORD0;
    float4 ProjPos : TEXCOORD1;
};

struct Output
{
    float4 color : SV_TARGET0;
    float4 depth : SV_TARGET3;
};

Output main(PixelInputType input)
{
    Output Out;
    Out.color = input.color;
    Out.depth = vector(input.ProjPos.z / input.ProjPos.w, input.ProjPos.w / 1000.f, 0.f, 0.f);
    return Out;
}