#pragma once
#include "GameObject.h"
class ColliderRenderer : public GameObject, public enable_shared_from_this<ColliderRenderer>
{
private:
	ColliderRenderer();
	ColliderRenderer(const ColliderRenderer& Rhs);

public:
	~ColliderRenderer();

private:
	HRESULT Initialize(PxGeometryType::Enum _geoType);
	HRESULT CreateBuffer(PxGeometryType::Enum _geoType);

public:
	// GameObject을(를) 통해 상속됨
	virtual int Update(float _TimeDelta) override;
	virtual void Render() override;

public:
	static shared_ptr<ColliderRenderer> Create(PxGeometryType::Enum _geoType);

private:
	shared_ptr<GraphicDevice> Graphic = nullptr;
	vector<GeometricPrimitive::VertexType> vertices;
	vector<uint16_t> indices;
	ComPtr<ID3D11Buffer> vertexBuffer = nullptr;
	ComPtr<ID3D11Buffer> indexBuffer = nullptr;

	PxGeometryType::Enum geoType;


};

