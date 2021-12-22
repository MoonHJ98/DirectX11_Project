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
	// ���� ���� ���� �� �������� �����մϴ�.
	unsigned int stride = sizeof(GeometricPrimitive::VertexType);
	unsigned int offset = 0;

	// ������ �� �� �ֵ��� �Է� ��������� ���� ���۸� Ȱ������ �����մϴ�.
	Graphic->GetDeviceContext()->IASetVertexBuffers(0, 1, vertexBuffer.GetAddressOf(), &stride, &offset);

	// ������ �� �� �ֵ��� �Է� ��������� �ε��� ���۸� Ȱ������ �����մϴ�.
	Graphic->GetDeviceContext()->IASetIndexBuffer(indexBuffer.Get(), DXGI_FORMAT_R16_UINT, 0);

	// �� ���ؽ� ���ۿ��� �������Ǿ���ϴ� ������Ƽ���� Ÿ���� �����Ѵ�.�� ��� ���θ���Ʈ�̴�.
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
