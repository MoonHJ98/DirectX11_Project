#pragma once

#include "Includes.h"

// Wrapper Class.
// 멤버를 감싸서 추가 기능을 제공하기 위한 클래스

// 정석은 ID3D11Buffer 상속받아서 사용.

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

	// 정점 정보를 담음				   
	void* Data = nullptr;
	// 정점의 갯수						 
	UINT Count = 0;
	// 정점 하나의 크기				   
	UINT Stride = 0;

	UINT Slot = 0;

	// 정점을 CPU에서 쓸 수 있는지
	bool CpuWrite = false;
	// 정점을 GPU에서 쓸 수 있는지
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