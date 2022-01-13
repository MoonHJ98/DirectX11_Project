#include "pch.h"
#include "Line.h"
#include "GraphicDevice.h"

Line::Line()
{
}

Line::Line(const Line & Rhs)
{
}

Line::~Line()
{
}

int Line::Update(float _TimeDelta)
{
	UpdateComponent(_TimeDelta);
	return 0;
}

void Line::Render()
{
	RenderComponent();
	RenderBuffer();
}

void Line::OnContact()
{
}

void Line::OnTrigger()
{
}

void Line::UpdateBuffer(vector<Vector3> _posVector, Vector4 _color)
{
	if (_posVector.size() <= 1)
		return;

	vertices.clear();
	indices.clear();

	if (vertexBuffer)
	{
		vertexBuffer->Release();
		vertexBuffer = nullptr;
	}

	if (indexBuffer)
	{
		indexBuffer->Release();
		indexBuffer = nullptr;
	}

	for (UINT i = 0; i < _posVector.size(); ++i)
	{
		vertices.push_back(VertexType(_posVector[i], _color));

		indices.push_back(i);

	}

	CreateStaticBuffer(Graphic->GetDevice(), &vertices[0], vertices.size(), sizeof(VertexType), D3D11_BIND_VERTEX_BUFFER, vertexBuffer.GetAddressOf());

	CreateStaticBuffer(Graphic->GetDevice(), &indices[0], indices.size(), sizeof(uint16_t), D3D11_BIND_INDEX_BUFFER, indexBuffer.GetAddressOf());
}

HRESULT Line::Initialize()
{
	Graphic = GraphicDevice::GetInstance();

	transform = Transform::Create(Transform::TRANSDESC());

	transform->SetObject(shared_from_this());

	D3D11_INPUT_ELEMENT_DESC InputLayout[] =
	{
		{ "POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 0, D3D11_INPUT_PER_VERTEX_DATA, 0 },
		{ "COLOR", 0, DXGI_FORMAT_R32G32B32A32_FLOAT, 0, D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_VERTEX_DATA, 0 }
	};

	shader = Shader::Create(InputLayout, sizeof(InputLayout), L"../Engine/LineVS.hlsl", L"../Engine/LinePS.hlsl");

	components[ComponentType::TRANSFORM] = transform;
	components[ComponentType::SHADER] = shader;

	return S_OK;
}

void Line::RenderBuffer()
{
	if (vertexBuffer == nullptr || indexBuffer == nullptr)
		return;

	// ���� ���� ���� �� �������� �����մϴ�.
	unsigned int stride = sizeof(VertexType);
	unsigned int offset = 0;

	// ������ �� �� �ֵ��� �Է� ��������� ���� ���۸� Ȱ������ �����մϴ�.
	Graphic->GetDeviceContext()->IASetVertexBuffers(0, 1, vertexBuffer.GetAddressOf(), &stride, &offset);

	// ������ �� �� �ֵ��� �Է� ��������� �ε��� ���۸� Ȱ������ �����մϴ�.
	Graphic->GetDeviceContext()->IASetIndexBuffer(indexBuffer.Get(), DXGI_FORMAT_R16_UINT, 0);

	// �� ���ؽ� ���ۿ��� �������Ǿ���ϴ� ������Ƽ���� Ÿ���� �����Ѵ�.�� ��� ���θ���Ʈ�̴�.
	Graphic->GetDeviceContext()->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_LINESTRIP);

	//Graphic->GetDeviceContext()->Draw(vertices.size(), 0);
	Graphic->GetDeviceContext()->DrawIndexed(indices.size(), 0, 0);
}

shared_ptr<Line> Line::Create()
{
	shared_ptr<Line> Instance(new Line());
	if (FAILED(Instance->Initialize()))
	{
		MSG_BOX("Failed to create Line.");
		return nullptr;
	}
	return Instance;
}
