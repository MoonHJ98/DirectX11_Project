#pragma once
#include "GameObject.h"
class DebugTreeRenderer : public GameObject, public enable_shared_from_this<DebugTreeRenderer>
{
private:
	DebugTreeRenderer();
	DebugTreeRenderer(const DebugTreeRenderer& Rhs);

public:
	~DebugTreeRenderer();

private:
	HRESULT Initialize(BoundingBox & _boundingBox);
	HRESULT InitializeBuffers(BoundingBox & _boundingBox);
	void RenderBuffer();

public:
	// GameObject을(를) 통해 상속됨
	virtual int Update(float _TimeDelta) override;
	virtual void Render() override;
	virtual void OnContact() override;
	virtual void OnTrigger() override;

public:
	static shared_ptr<DebugTreeRenderer> Create(BoundingBox & _boundingBox);

private:
	ComPtr<ID3D11Buffer> vertexBuffer = nullptr;
	ComPtr<ID3D11Buffer> indexBuffer = nullptr;
	int indexCount = 0;
};

