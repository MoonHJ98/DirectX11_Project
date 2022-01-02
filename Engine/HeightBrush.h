#pragma once
#include "GameObject.h"


class HeightBrush : public GameObject, public enable_shared_from_this<HeightBrush>
{
private:
	struct HeightMapType
	{
		float x, y, z;
		float tu, tv;
		float nx, ny, nz;
	};
	struct VectorType
	{
		float x, y, z;
	};
private:
	HeightBrush();
	HeightBrush(const HeightBrush& Rhs);

public:
	~HeightBrush();

public:
	// GameObject을(를) 통해 상속됨
	virtual int Update(float _TimeDelta) override;
	virtual void Render() override;
	virtual void OnContact() override;
	virtual void OnTrigger() override;

private:
	HRESULT Initialize(const char* _heightMapPath);
	bool LoadHeightTexture(const char* _heightMapPath);
	void NormalizeHeightMap();
	bool CalculateNormals();
	HRESULT InitializeBuffer();
	void RenderBuffer();

public:
	void SetPosition(BrushDesc& _brushDesc);
	float* RaiseHeight(Vector3 * _pos);

public:
	static shared_ptr<HeightBrush> Create(const char* _heightMapPath);

private:
	HeightMapType* heightMap = nullptr;

	int heightMapHeight = 0;
	int heightMapWidth = 0;
	UINT vertexCount = 0;
	TerrainVertexType* vertices = nullptr;
	ComPtr<ID3D11Buffer> vertexBuffer;

	int brushHeight = 0;
	int brushWidth = 0;

};

