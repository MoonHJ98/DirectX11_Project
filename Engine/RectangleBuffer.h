#pragma once

#include "Includes.h"

class GraphicDevice;

class RectangleBuffer
{
private:
	struct VertexType
	{
		XMFLOAT3 position;
		XMFLOAT2 texture;
	};
private:
	RectangleBuffer();
	RectangleBuffer(const RectangleBuffer& Rhs);

public:
	~RectangleBuffer();

private:
	HRESULT Initialize();
	HRESULT InitializeBuffer();
	void	RenderBuffer();

public:
	void Render();
	int	 GetIndexCount() { return indexCount; }


public:
	static shared_ptr<RectangleBuffer> Create();

private:
	shared_ptr<GraphicDevice> Graphic = nullptr;
	ComPtr<ID3D11Buffer> vertexBuffer = nullptr;
	ComPtr<ID3D11Buffer> indexBuffer = nullptr;
	int vertexCount = 0;
	int indexCount = 0;
};

