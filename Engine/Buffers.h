#pragma once

#include "Includes.h"

// Wrapper Class.
// ����� ���μ� �߰� ����� �����ϱ� ���� Ŭ����

// ������ ID3D11Buffer ��ӹ޾Ƽ� ���.

class GraphicDevice;

// VertexBuffer.
class VertexBuffer
{
private:
	VertexBuffer();
	VertexBuffer(const VertexBuffer& Rhs);

public:
	~VertexBuffer();

private:
	HRESULT								 Initialize(void* _Data, UINT _Count, UINT _Stride, UINT _Slot = 0, bool _CpuWrite = false, bool _GpuWrite = false);

public:
	UINT								 GetCount() { return Count; }
	UINT								 GetStride() { return Stride; }
	ID3D11Buffer*						 GetBuffer() { return Buffer.Get(); }

public:
	void Render();

public:
	static shared_ptr<VertexBuffer>		 Create(void* _Data, UINT _Count, UINT _Stride, UINT _Slot = 0, bool _CpuWrite = false, bool _GpuWrite = false);

private:
	shared_ptr<GraphicDevice> Graphic = nullptr;

	ComPtr<ID3D11Buffer> Buffer = nullptr;

	// ���� ������ ����				   
	void* Data = nullptr;
	// ������ ����						 
	UINT Count = 0;
	// ���� �ϳ��� ũ��				   
	UINT Stride = 0;

	UINT Slot = 0;

	// ������ CPU���� �� �� �ִ���
	bool CpuWrite = false;
	// ������ GPU���� �� �� �ִ���
	bool GpuWrite = false;

};

// Index Buffer.

class IndexBuffer
{
private:
	IndexBuffer();
	IndexBuffer(const IndexBuffer& Rhs);

public:
	~IndexBuffer();

private:
	HRESULT							Initialize(void* _Data, UINT _Count);

public:
	UINT							GetCount() { return Count; }
	ID3D11Buffer*					GetBuffer() { return Buffer.Get(); }

public:
	void							Render();

public:
	static shared_ptr<IndexBuffer>  Create(void* _Data, UINT _Count);

private:
	shared_ptr<GraphicDevice> Graphic = nullptr;
	ComPtr<ID3D11Buffer> Buffer = nullptr;

	void* Data = nullptr;
	UINT Count = 0;
};