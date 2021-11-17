#include "pch.h"
#include "Sphere.h"
#include "GraphicDevice.h"

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
	
	GeometricPrimitive::CreateSphere(vertices, indices);

	CreateStaticBuffer(Graphic->GetDevice(), &vertices[0], vertices.size(), sizeof(GeometricPrimitive::VertexType), D3D11_BIND_VERTEX_BUFFER, vertexBuffer.GetAddressOf());
	CreateStaticBuffer(Graphic->GetDevice(), &indices[0], indices.size(), sizeof(uint16_t), D3D11_BIND_INDEX_BUFFER, indexBuffer.GetAddressOf());
	
	return S_OK;
}

void Sphere::Render()
{
	// 정점 버퍼 보폭 및 오프셋을 설정합니다.
	unsigned int stride = sizeof(GeometricPrimitive::VertexType);
	unsigned int offset = 0;

	// 렌더링 할 수 있도록 입력 어셈블러에서 정점 버퍼를 활성으로 설정합니다.
	Graphic->GetDeviceContext()->IASetVertexBuffers(0, 1, vertexBuffer.GetAddressOf(), &stride, &offset);

	// 렌더링 할 수 있도록 입력 어셈블러에서 인덱스 버퍼를 활성으로 설정합니다.
	Graphic->GetDeviceContext()->IASetIndexBuffer(indexBuffer.Get(), DXGI_FORMAT_R32_UINT, 0);

	// 이 버텍스 버퍼에서 렌더링되어야하는 프리미티브의 타입을 설정한다.이 경우 라인리스트이다.
	Graphic->GetDeviceContext()->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);

	Graphic->GetDeviceContext()->DrawIndexed(indices.size(), 0, 0);
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
