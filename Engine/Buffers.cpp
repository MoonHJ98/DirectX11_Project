#include "pch.h"
#include "Buffers.h"
#include "GraphicDevice.h"


// VertexBuffer //////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

VertexBuffer::VertexBuffer()
{
}

VertexBuffer::VertexBuffer(const VertexBuffer & Rhs)
{
}

VertexBuffer::~VertexBuffer()
{
}

HRESULT VertexBuffer::Initialize(void * _Data, UINT _Count, UINT _Stride, UINT _Slot, bool _CpuWrite, bool _GpuWrite)
{
	Data = _Data;
	Count = _Count;
	Stride = _Stride;
	Slot = _Slot;
	CpuWrite = _CpuWrite;
	GpuWrite = _GpuWrite;

	D3D11_BUFFER_DESC desc;
	ZeroMemory(&desc, sizeof(D3D11_BUFFER_DESC));
	desc.ByteWidth = Stride * Count;
	desc.BindFlags = D3D11_BIND_VERTEX_BUFFER;

	if (CpuWrite == false && GpuWrite == false)
	{
		desc.Usage = D3D11_USAGE_IMMUTABLE; // ���� �Ұ�. ��������. �ӵ��� ���� ����
	}
	else if (CpuWrite == true && GpuWrite == false) // ������ �ٲ�� �ϴ� ��� ex) ������ ������������ �� ���
	{
		desc.Usage = D3D11_USAGE_DYNAMIC; // cpu ���� ����(Gpu�� ������ ����), Gpu �б� ���� (������������ ���� ����)
		desc.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE; // Flag�� ������� ���� ����.
	}
	else if (CpuWrite == false && GpuWrite == true) // Gpu(shader)���� ������ Cpu�� ���� �� ���
	{
		desc.Usage = D3D11_USAGE_DEFAULT; // Cpu�б�, Gpu ����. ���ܷ� UpdateSubResource�� ���� Cpu���Ⱑ ����.(�Ϲ������� ��� X map, unmap ���)
	}
	else
	{
		desc.Usage = D3D11_USAGE_STAGING; // ��� ���� ����. ���� ����
		desc.CPUAccessFlags = D3D11_CPU_ACCESS_READ | D3D11_CPU_ACCESS_WRITE;

	}

	D3D11_SUBRESOURCE_DATA subResource = {};
	subResource.pSysMem = Data;

	

	return Graphic->GetDevice()->CreateBuffer(&desc, &subResource, Buffer.GetAddressOf());
}

void VertexBuffer::Render()
{
	UINT offest = 0;
	Graphic->GetDeviceContext()->IAGetVertexBuffers(Slot, 1, Buffer.GetAddressOf(), &Stride, &offest);

}

shared_ptr<VertexBuffer> VertexBuffer::Create(void* _Data, UINT _Count, UINT _Stride, UINT _Slot, bool _CpuWrite, bool _GpuWrite)
{
	shared_ptr<VertexBuffer> Instance(new VertexBuffer());

	Instance->Graphic = GraphicDevice::GetInstance();

	if (FAILED(Instance->Initialize(_Data, _Count, _Stride, _Slot, _CpuWrite, _GpuWrite)))
	{
		MSG_BOX("Failed to create VertexBuffer.");
		return nullptr;
	}

	return Instance;
	
}

// IndexBuffer //////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

IndexBuffer::IndexBuffer()
{
}

IndexBuffer::IndexBuffer(const IndexBuffer & Rhs)
{
}

IndexBuffer::~IndexBuffer()
{
}

HRESULT IndexBuffer::Initialize(void * _Data, UINT _Count)
{
	Data = _Data;
	Count = _Count;

	D3D11_BUFFER_DESC desc;
	ZeroMemory(&desc, sizeof(D3D11_BUFFER_DESC));

	desc.ByteWidth = sizeof(UINT) * Count;
	desc.BindFlags = D3D11_BIND_INDEX_BUFFER;
	desc.Usage = D3D11_USAGE_IMMUTABLE;

	D3D11_SUBRESOURCE_DATA subResource = {};
	subResource.pSysMem = Data;

	return Graphic->GetDevice()->CreateBuffer(&desc, &subResource, Buffer.GetAddressOf());;
}

void IndexBuffer::Render()
{
	Graphic->GetDeviceContext()->IASetIndexBuffer(Buffer.Get(), DXGI_FORMAT_R32_UINT, 0);
}

shared_ptr<IndexBuffer> IndexBuffer::Create(void * _Data, UINT _Count)
{
	shared_ptr<IndexBuffer> Instance(new IndexBuffer());

	Instance->Graphic = GraphicDevice::GetInstance();

	if (FAILED(Instance->Initialize(_Data, _Count)))
	{
		MSG_BOX("Failed to create IndexBuffer.");
		return nullptr;
	}
	return Instance;
}
