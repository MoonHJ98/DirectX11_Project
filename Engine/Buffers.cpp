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
		desc.Usage = D3D11_USAGE_IMMUTABLE; // 접근 불가. 렌더링용. 속도가 가장 빠름
	}
	else if (CpuWrite == true && GpuWrite == false) // 정점을 바꿔야 하는 경우 ex) 지형을 오르락내리락 할 경우
	{
		desc.Usage = D3D11_USAGE_DYNAMIC; // cpu 쓰기 가능(Gpu로 보내기 가능), Gpu 읽기 가능 (렌더링에서만 접근 가능)
		desc.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE; // Flag를 열어줘야 쓰기 가능.
	}
	else if (CpuWrite == false && GpuWrite == true) // Gpu(shader)에서 쓴것을 Cpu에 보낼 때 사용
	{
		desc.Usage = D3D11_USAGE_DEFAULT; // Cpu읽기, Gpu 쓰기. 예외로 UpdateSubResource를 통해 Cpu쓰기가 가능.(일반적으로 사용 X map, unmap 사용)
	}
	else
	{
		desc.Usage = D3D11_USAGE_STAGING; // 모든 것이 가능. 가장 느림
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
