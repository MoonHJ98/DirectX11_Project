#pragma once

#include "Component.h"

class GraphicDevice;

class RectangleBuffer : public Component
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
	virtual int Update(float _timeDelta) override;
	virtual void Render() override;
	int	 GetIndexCount() { return indexCount; }


public:
	static shared_ptr<RectangleBuffer> Create();

private:
	shared_ptr<GraphicDevice> Graphic = nullptr;
	ComPtr<ID3D11Buffer> vertexBuffer = nullptr;
	ComPtr<ID3D11Buffer> indexBuffer = nullptr;
	ComPtr<ID3D11ShaderResourceView> resourceView = nullptr;
	int vertexCount = 0;
	int indexCount = 0;


};

