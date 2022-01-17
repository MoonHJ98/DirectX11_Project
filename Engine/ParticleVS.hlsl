cbuffer MatrixBuffer : register(b0)
{
    matrix worldMatrix;
    matrix viewMatrix;
    matrix projectionMatrix;
};


struct VertexInputType
{
    float3 position : POSITION;
    float2 tex : TEXCOORD;
    float3 color : COLOR;
};

struct PixelInputType
{
    float4 position : SV_POSITION;
    float2 tex : TEXCOORD;
    float3 color : COLOR;
    float4 ProjPos : TEXCOORD1;

};


PixelInputType main(VertexInputType input)
{
    PixelInputType output = (PixelInputType) 0;
    

	// 월드, 뷰 및 투영 행렬에 대한 정점의 위치를 계산합니다.
    output.position = mul(float4(input.position, 1.f), worldMatrix);
    output.position = mul(output.position, viewMatrix);
    output.position = mul(output.position, projectionMatrix);
    
	// 픽셀 쉐이더의 텍스처 좌표를 설정합니다
    output.tex = input.tex;
    output.color = input.color;

    output.ProjPos = output.position;
    
    return output;
}