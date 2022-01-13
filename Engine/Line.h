#pragma once

#include "GameObject.h"

class Management;

class Line : public GameObject, public enable_shared_from_this<Line>
{
	struct VertexType
	{
		VertexType(Vector3 _pos, Vector4 _color)
		{
			pos = _pos;
			color = _color;
		}
		Vector3 pos;
		Vector4 color;
	};
private:
	Line();
	Line(const Line& Rhs);

public:
	~Line();

public:
	// GameObject을(를) 통해 상속됨
	virtual int Update(float _TimeDelta) override;
	virtual void Render() override;
	virtual void OnContact() override;
	virtual void OnTrigger() override;

public:
	void UpdateBuffer(vector<Vector3> _posVector, Vector4 _color);

private:
	HRESULT Initialize();
	void RenderBuffer();

public:
	static shared_ptr<Line> Create();

private:
	shared_ptr<Management> Manage = nullptr;
	vector<VertexType> vertices;
	vector<uint16_t> indices;
	ComPtr<ID3D11Buffer> vertexBuffer;
	ComPtr<ID3D11Buffer> indexBuffer;
};

