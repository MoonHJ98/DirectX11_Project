#include "pch.h"
#include "ColliderRenderer.h"
#include "GraphicDevice.h"
#include "Capsule.h"
#include "Shader.h"
#include "Transform.h"
#include "Collider.h"

ColliderRenderer::ColliderRenderer()
{
}

ColliderRenderer::ColliderRenderer(const ColliderRenderer & Rhs)
{
}

ColliderRenderer::~ColliderRenderer()
{
}

HRESULT ColliderRenderer::Initialize(shared_ptr<GameObject> _object, shared_ptr<Collider> _collider, PxGeometryType::Enum _geoType)
{
	object = _object;
	collider = _collider;
	
	geoType = _geoType;
	Graphic = GraphicDevice::GetInstance();

	CreateBuffer(_geoType);

	transform = Transform::Create(Transform::TRANSDESC());

	transform->SetObject(shared_from_this());
	objectTransform = dynamic_pointer_cast<Transform>(object.lock()->GetComponent(ComponentType::TRANSFORM));
	Vector3 position = objectTransform.lock()->GetState(Transform::POSITION);
	auto localTransform = collider.lock()->GetLocalTransform();
	position += Vector3(localTransform.p.x, localTransform.p.y, localTransform.p.z);

	transform->SetState(Transform::POSITION, position);


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

HRESULT ColliderRenderer::CreateBuffer(PxGeometryType::Enum _geoType)
{
	switch (_geoType)
	{
	case physx::PxGeometryType::eSPHERE:
		GeometricPrimitive::CreateSphere(vertices, indices, 10, 8, false);
		break;

	case physx::PxGeometryType::ePLANE:
		break;

	case physx::PxGeometryType::eCAPSULE:
		Capsule::CreateCapsule(vertices, indices, 5.f, 10.f);
		break;

	case physx::PxGeometryType::eBOX:
		GeometricPrimitive::CreateBox(vertices, indices, XMFLOAT3(10.f, 10.f, 10.f), false);
		break;

	case physx::PxGeometryType::eHEIGHTFIELD:
		break;

	case physx::PxGeometryType::eINVALID:
		break;
	default:
		break;
	}

	for (UINT i = 0; i < vertices.size(); ++i)
	{
		vertices[i].normal = Vector3(0.f, 1.f, 0.f);
	}

	CreateStaticBuffer(Graphic->GetDevice(), &vertices[0], vertices.size(), sizeof(GeometricPrimitive::VertexType), D3D11_BIND_VERTEX_BUFFER, vertexBuffer.GetAddressOf());

	if (!indices.empty())
		CreateStaticBuffer(Graphic->GetDevice(), &indices[0], indices.size(), sizeof(uint16_t), D3D11_BIND_INDEX_BUFFER, indexBuffer.GetAddressOf());

	return S_OK;
}

int ColliderRenderer::Update(float _TimeDelta)
{
	Vector3 position = objectTransform.lock()->GetState(Transform::POSITION);
	auto localTransform = collider.lock()->GetLocalTransform();
	position += Vector3(localTransform.p.x, localTransform.p.y, localTransform.p.z);

	transform->SetState(Transform::POSITION, position);

	UpdateComponent(_TimeDelta);

	return 0;
}

void ColliderRenderer::Render()
{
	RenderComponent();
	RenderBuffer();
}

void ColliderRenderer::RenderBuffer()
{
	// 정점 버퍼 보폭 및 오프셋을 설정합니다.
	unsigned int stride = sizeof(GeometricPrimitive::VertexType);
	unsigned int offset = 0;

	// 렌더링 할 수 있도록 입력 어셈블러에서 정점 버퍼를 활성으로 설정합니다.
	Graphic->GetDeviceContext()->IASetVertexBuffers(0, 1, vertexBuffer.GetAddressOf(), &stride, &offset);

	// 렌더링 할 수 있도록 입력 어셈블러에서 인덱스 버퍼를 활성으로 설정합니다.
	Graphic->GetDeviceContext()->IASetIndexBuffer(indexBuffer.Get(), DXGI_FORMAT_R16_UINT, 0);

	// 이 버텍스 버퍼에서 렌더링되어야하는 프리미티브의 타입을 설정한다.이 경우 라인리스트이다.
	Graphic->GetDeviceContext()->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_LINESTRIP);

	if (indices.empty())
		Graphic->GetDeviceContext()->Draw(vertices.size(), 0);
	else
		Graphic->GetDeviceContext()->DrawIndexed(indices.size(), 0, 0);

}

shared_ptr<ColliderRenderer> ColliderRenderer::Create(shared_ptr<GameObject> _object, shared_ptr<Collider> _collider, PxGeometryType::Enum _geoType)
{
	shared_ptr<ColliderRenderer> Instance(new ColliderRenderer());
	if (FAILED(Instance->Initialize(_object, _collider, _geoType)))
	{
		MSG_BOX("Failed to create ColliderRenderer.");
		return nullptr;
	}
	return Instance;
}

void ColliderRenderer::OnContact()
{
}

void ColliderRenderer::OnTrigger()
{
}
