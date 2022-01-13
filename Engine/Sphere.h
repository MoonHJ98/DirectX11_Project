#pragma once

#include "Component.h"

class GraphicDevice;

class Sphere : public Component
{
private:
	Sphere();
	Sphere(const Sphere& Rhs);

public:
	~Sphere();

private:
	HRESULT Initialize(float _diameter, Vector3 _color);

public:
	// Component을(를) 통해 상속됨
	virtual int Update(float _timeDelta) override;
	virtual void RenderInspector() override;
	virtual void Render() override;

public:
	static shared_ptr<Sphere> Create(float _diameter, Vector3 _color = Vector3(-1.f, -1.f, -1.f));

private:
	shared_ptr<GraphicDevice> Graphic = nullptr;
	
	vector<GeometricPrimitive::VertexType> vertices;
	vector<uint16_t> indices;

	ComPtr<ID3D11Buffer> vertexBuffer = nullptr;
	ComPtr<ID3D11Buffer> indexBuffer = nullptr;

};

