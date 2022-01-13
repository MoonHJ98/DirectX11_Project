struct PixelInputType
{
	float4 position : SV_POSITION;
	float4 color : COLOR;
};

struct Output
{
    float4 color : SV_TARGET0;
    float4 normal : SV_TARGET1;
    float4 Specular : SV_TARGET2;
    float4 depth : SV_TARGET3;
    float4 lightViewPosition : SV_TARGET4;
};


Output main(PixelInputType input)
{
    Output Out;
    Out.color = input.color;
    Out.normal = float4(0.f, 0.f, 0.f, 0.f);
    Out.depth = float4(0.f, 0.f, 0.f, 0.f);
    Out.Specular = float4(0.f, 0.f, 0.f, 0.f);
    Out.lightViewPosition = float4(0.f, 0.f, 0.f, 0.f);
    return Out;
}