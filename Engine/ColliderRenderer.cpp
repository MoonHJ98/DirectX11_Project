#include "pch.h"
#include "ColliderRenderer.h"
#include "GraphicDevice.h"

ColliderRenderer::ColliderRenderer()
{
}

ColliderRenderer::ColliderRenderer(const ColliderRenderer & Rhs)
{
}

ColliderRenderer::~ColliderRenderer()
{
}

HRESULT ColliderRenderer::Initialize(PxGeometryType::Enum _geoType)
{
	geoType = _geoType;
	Graphic = GraphicDevice::GetInstance();
	
	//CreateBuffer();
	return S_OK;
}

HRESULT ColliderRenderer::CreateBuffer(PxGeometryType::Enum _geoType)
{
	switch (_geoType)
	{
	case physx::PxGeometryType::eSPHERE:
		GeometricPrimitive::CreateSphere(vertices, indices, 10.f, 16, false);
		break;
	case physx::PxGeometryType::ePLANE:
		GeometricPrimitive::CreateSphere(vertices, indices, 10.f, 16, false);
		break;
	case physx::PxGeometryType::eCAPSULE:
		break;
	case physx::PxGeometryType::eBOX:
		break;
	case physx::PxGeometryType::eCONVEXMESH:
		break;
	case physx::PxGeometryType::eTRIANGLEMESH:
		break;
	case physx::PxGeometryType::eHEIGHTFIELD:
		break;
	case physx::PxGeometryType::eGEOMETRY_COUNT:
		break;
	case physx::PxGeometryType::eINVALID:
		break;
	default:
		break;
	}

	CreateStaticBuffer(Graphic->GetDevice(), &vertices[0], vertices.size(), sizeof(GeometricPrimitive::VertexType), D3D11_BIND_VERTEX_BUFFER, vertexBuffer.GetAddressOf());
	CreateStaticBuffer(Graphic->GetDevice(), &indices[0], indices.size(), sizeof(uint16_t), D3D11_BIND_INDEX_BUFFER, indexBuffer.GetAddressOf());

	return E_NOTIMPL;
}

int ColliderRenderer::Update(float _TimeDelta)
{
	return 0;
}

void ColliderRenderer::Render()
{
}

shared_ptr<ColliderRenderer> ColliderRenderer::Create(PxGeometryType::Enum _geoType)
{
	shared_ptr<ColliderRenderer> Instance(new ColliderRenderer());
	if (FAILED(Instance->Initialize(_geoType)))
	{
		MSG_BOX("Failed to create ColliderRenderer.");
		return nullptr;
	}
	return Instance;
}
