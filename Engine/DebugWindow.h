#pragma once

#include "Includes.h"

class GraphicDevice;

class DebugWindow
{
private:
	struct VertexType
	{
		XMFLOAT3 position;
		XMFLOAT2 texture;
	};

public:
	DebugWindow();
	DebugWindow(const DebugWindow& Rhs);
	~DebugWindow();

private:
	HRESULT Initialize(int _screenWidth, int _screenHeight, int _bitmapWidth, int _bitmapHeight);

public:
	void Render();
	int GetIndexCount();

private:
	HRESULT InitializeBuffers();
	void	RenderBuffers();

public:
	static shared_ptr<DebugWindow> Create(int _screenWidth, int _screenHeight, int _bitmapWidth, int _bitmapHeight);

private:
	shared_ptr<GraphicDevice> Graphic = nullptr;
	ComPtr<ID3D11Buffer> vertexBuffer = nullptr;
	ComPtr<ID3D11Buffer> indexBuffer = nullptr;
	int vertexCount = 0;
	int indexCount = 0;
	int screenWidth = 0;
	int screenHeight = 0;
	int bitmapWidth = 0;
	int bitmapHeight = 0;
};

