#pragma once

#include "Includes.h"

class GraphicDevice;
class RectangleBuffer;
class Transform;
class Shader;

class RenderTarget
{
private:
	RenderTarget();
	RenderTarget(const RenderTarget& Rhs);

public:
	~RenderTarget();
	
private:
	HRESULT Initialize(Vector3 _position, Vector3 bitmapScale, Vector4 _ClearColor);

public:
	void Render(ID3D11ShaderResourceView* _ShaderResourceView);


public:
	static shared_ptr<RenderTarget> Create(Vector3 _position, Vector3 bitmapScale, Vector4 _ClearColor);

private:


	shared_ptr<GraphicDevice>           Graphic = nullptr;
	shared_ptr<RectangleBuffer>         rectangleBuffer = nullptr;
	shared_ptr<Transform>				transform = nullptr;
	shared_ptr<Shader>					shader = nullptr;

	Vector4 ClearColor;

};

