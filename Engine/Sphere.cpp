#include "pch.h"
#include "Sphere.h"
#include "GraphicDevice.h"
#include "Capsule.h"

Sphere::Sphere()
{
}

Sphere::Sphere(const Sphere & Rhs)
{
}

Sphere::~Sphere()
{
}

HRESULT Sphere::Initialize()
{
	Graphic = GraphicDevice::GetInstance();
	
	
	//GeometricPrimitive::CreateSphere(vertices, indices, 10.f, 16, false);

	Capsule::CreateCapsule(vertices, indices, 5.f, 10.f);
	
	CreateStaticBuffer(Graphic->GetDevice(), &vertices[0], vertices.size(), sizeof(GeometricPrimitive::VertexType), D3D11_BIND_VERTEX_BUFFER, vertexBuffer.GetAddressOf());
	//CreateStaticBuffer(Graphic->GetDevice(), &indices[0], indices.size(), sizeof(uint16_t), D3D11_BIND_INDEX_BUFFER, indexBuffer.GetAddressOf());
	
	return S_OK;
}

void Sphere::Render()
{
	// ���� ���� ���� �� �������� �����մϴ�.
	unsigned int stride = sizeof(GeometricPrimitive::VertexType);
	unsigned int offset = 0;

	// ������ �� �� �ֵ��� �Է� ��������� ���� ���۸� Ȱ������ �����մϴ�.
	Graphic->GetDeviceContext()->IASetVertexBuffers(0, 1, vertexBuffer.GetAddressOf(), &stride, &offset);

	// ������ �� �� �ֵ��� �Է� ��������� �ε��� ���۸� Ȱ������ �����մϴ�.
	Graphic->GetDeviceContext()->IASetIndexBuffer(indexBuffer.Get(), DXGI_FORMAT_R16_UINT, 0);

	// �� ���ؽ� ���ۿ��� �������Ǿ���ϴ� ������Ƽ���� Ÿ���� �����Ѵ�.�� ��� ���θ���Ʈ�̴�.
	Graphic->GetDeviceContext()->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_LINELIST);

	Graphic->GetDeviceContext()->Draw(vertices.size(), 0);
	//Graphic->GetDeviceContext()->DrawIndexed(indices.size(), 0, 0);
}

shared_ptr<Sphere> Sphere::Create()
{
	shared_ptr<Sphere> Instance(new Sphere());
	if (FAILED(Instance->Initialize()))
	{
		MSG_BOX("Failed to create Sphere.");
		return nullptr;
	}
	return Instance;
}
