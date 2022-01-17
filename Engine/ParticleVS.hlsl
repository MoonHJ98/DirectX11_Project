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
    

	// ����, �� �� ���� ��Ŀ� ���� ������ ��ġ�� ����մϴ�.
    output.position = mul(float4(input.position, 1.f), worldMatrix);
    output.position = mul(output.position, viewMatrix);
    output.position = mul(output.position, projectionMatrix);
    
	// �ȼ� ���̴��� �ؽ�ó ��ǥ�� �����մϴ�
    output.tex = input.tex;
    output.color = input.color;

    output.ProjPos = output.position;
    
    return output;
}