#include "pch.h"
#include "DebugTreeRenderer.h"
#include "GraphicDevice.h"


DebugTreeRenderer::DebugTreeRenderer()
{
}

DebugTreeRenderer::DebugTreeRenderer(const DebugTreeRenderer & Rhs)
{
}

DebugTreeRenderer::~DebugTreeRenderer()
{
}

HRESULT DebugTreeRenderer::Initialize(BoundingBox & _boundingBox)
{
	Graphic = GraphicDevice::GetInstance();
	InitializeBuffers(_boundingBox);

	transform = Transform::Create(Transform::TRANSDESC());

	transform->SetObject(shared_from_this());



	D3D11_INPUT_ELEMENT_DESC InputLayout[] =
	{
		{ "POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 0, D3D11_INPUT_PER_VERTEX_DATA, 0 },
		{ "COLOR", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_VERTEX_DATA, 0 },
		{ "TEXCOORD", 0, DXGI_FORMAT_R32G32_FLOAT, 0, D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_VERTEX_DATA, 0 }

	};

	shader = Shader::Create(InputLayout, sizeof(InputLayout), L"../Engine/ColliderVS.hlsl", L"../Engine/ColliderPS.hlsl");

	components[ComponentType::SHADER] = shader;
	components[ComponentType::TRANSFORM] = transform;
	return S_OK;
}

HRESULT DebugTreeRenderer::InitializeBuffers(BoundingBox & _boundingBox)
{
	VertexPositionNormalTexture vertex;
	vector<VertexPositionNormalTexture> vertices;
	vector<uint16_t> indices;

	Vector3* corners = new Vector3[8];
	_boundingBox.GetCorners(corners);
	auto RearLowerLeft = corners[0];
	auto RearLowerRight = corners[1];
	auto RearUpperRight = corners[2];
	auto RearUpperLeft = corners[3];
	auto FrontLowerLeft = corners[4];
	auto FrontLowerRight = corners[5];
	auto FrontUpperRight = corners[6];
	auto FrontUpperLeft = corners[7];

	for (UINT i = 0; i < 8; ++i)
	{
		vertex.position = corners[i];
		vertex.normal = Vector3(0.f, 1.f, 0.f); // 색상
		vertex.textureCoordinate = Vector2();
		vertices.push_back(vertex);
	}

	indices.push_back(7);
	indices.push_back(6);
	indices.push_back(5);
	indices.push_back(4);
	indices.push_back(7);
	indices.push_back(4);
	indices.push_back(0);
	indices.push_back(3);
	indices.push_back(7);
	indices.push_back(6);
	indices.push_back(5);
	indices.push_back(1);
	indices.push_back(2);
	indices.push_back(6);
	indices.push_back(2);
	indices.push_back(3);
	indices.push_back(0);
	indices.push_back(1);


	if (FAILED(CreateStaticBuffer(GraphicDevice::GetInstance()->GetDevice(), &vertices[0], vertices.size(), sizeof(VertexPositionNormalTexture), D3D11_BIND_VERTEX_BUFFER, vertexBuffer.GetAddressOf())))
	{
		MSG_BOX("Failed to create vertex buffer for DebugTreeRenderer.");
		return E_FAIL;
	}


	if (FAILED(CreateStaticBuffer(GraphicDevice::GetInstance()->GetDevice(), &indices[0], indices.size(), sizeof(uint16_t), D3D11_BIND_INDEX_BUFFER, indexBuffer.GetAddressOf())))
	{
		MSG_BOX("Failed to create index buffer for DebugTreeRenderer.");
		return E_FAIL;
	}
	indexCount = indices.size();


	return S_OK;
}

void DebugTreeRenderer::RenderBuffer()
{
	// 정점 버퍼 보폭 및 오프셋을 설정합니다.
	unsigned int stride = sizeof(VertexPositionNormalTexture);
	unsigned int offset = 0;

	// 렌더링 할 수 있도록 입력 어셈블러에서 정점 버퍼를 활성으로 설정합니다.
	Graphic->GetDeviceContext()->IASetVertexBuffers(0, 1, vertexBuffer.GetAddressOf(), &stride, &offset);

	// 렌더링 할 수 있도록 입력 어셈블러에서 인덱스 버퍼를 활성으로 설정합니다.
	Graphic->GetDeviceContext()->IASetIndexBuffer(indexBuffer.Get(), DXGI_FORMAT_R16_UINT, 0);

	// 이 버텍스 버퍼에서 렌더링되어야하는 프리미티브의 타입을 설정한다.이 경우 라인리스트이다.
	Graphic->GetDeviceContext()->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_LINESTRIP);
	
	Graphic->GetDeviceContext()->DrawIndexed(indexCount, 0, 0);
}

int DebugTreeRenderer::Update(float _TimeDelta)
{
	UpdateComponent(_TimeDelta);
	return 0;
}

void DebugTreeRenderer::Render()
{
	RenderComponent();
	RenderBuffer();
}

void DebugTreeRenderer::OnContact()
{
}

void DebugTreeRenderer::OnTrigger()
{
}

shared_ptr<DebugTreeRenderer> DebugTreeRenderer::Create(BoundingBox & _boundingBox)
{
	shared_ptr<DebugTreeRenderer> Instance(new DebugTreeRenderer());
	if (FAILED(Instance->Initialize(_boundingBox)))
	{
		MSG_BOX("Failed to create DebugTreeRenderer.");
		return nullptr;
	}
	return Instance;
}
