#pragma once

#include "Includes.h"

class GraphicDevice;

class OrthoWindow
{
private:
	struct VertexType
	{
		Vector3 position;
		Vector2 texture;
	};

private:
	OrthoWindow();
	OrthoWindow(const OrthoWindow& Rhs);

public:
	~OrthoWindow();

private:
	HRESULT Initialize();
	HRESULT InitializeBuffers(int _windowWidth, int _windowHeight);
	void	RenderBuffers();

public:
	void Render();
	int GetIndexCount();

public:
	static shared_ptr<OrthoWindow> Create();

private:
	shared_ptr<GraphicDevice> Graphic = nullptr;

	ComPtr<ID3D11Buffer> vertexBuffer = nullptr;
	ComPtr<ID3D11Buffer> indexBuffer = nullptr;
	int vertexCount = 0;
	int indexCount = 0;



};

