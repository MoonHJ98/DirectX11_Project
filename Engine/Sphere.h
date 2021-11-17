#pragma once

#include "Includes.h"

class GraphicDevice;

class Sphere
{
private:
	Sphere();
	Sphere(const Sphere& Rhs);

public:
	~Sphere();

private:
	HRESULT Initialize();

public:
	void Render();

public:
	static shared_ptr<Sphere> Create();

private:
	shared_ptr<GraphicDevice> Graphic = nullptr;
	
	vector<GeometricPrimitive::VertexType> vertices;
	vector<uint16_t> indices;

	ComPtr<ID3D11Buffer> vertexBuffer = nullptr;
	ComPtr<ID3D11Buffer> indexBuffer = nullptr;

};

