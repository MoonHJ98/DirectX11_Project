#pragma once

#include "Includes.h"

class GraphicDevice;
class Shader;
class Transform;
class RectangleBuffer;


class Light
{
private:
	Light();
	Light(const Light& Rhs);

public:
	~Light();

public:
	struct LightBufferType //16바이트의 배수로 패딩
	{
		Vector4 DiffuseColor;
		Vector4 AmbientColor;
		Vector4 SpecularColor;
		Vector3 Direction;
		float SpecularPower;
		//float Padding = 0.f;
	};


public:
	LIGHTDESC& GetLightInfo() { return LightInfo; }
	void SetLightDirection(Vector3 _direction) { LightInfo.Direction = _direction; }

public:
	void Render();
	Matrix* GetViewMatrix() { return &ViewMatrix; }
	Matrix* GetProjectionMatrix() { return &ProjectMatrix; }
	Matrix* GetOrthoMatrix() { return &OrthoMatrix; }

private:
	HRESULT Initialize(LIGHTDESC _LightInfo);
	void CreateViewMatrix();
	void CreateProjMatrix();

public:
	static shared_ptr<Light> Create(LIGHTDESC _LightInfo);

private:
	shared_ptr<GraphicDevice> Graphic = nullptr;
	LIGHTDESC LightInfo;

	shared_ptr<ConstantBuffer<LightBufferType>> LightBuffer = nullptr;

	shared_ptr<RectangleBuffer> rectangleBuffer = nullptr;
	shared_ptr<Transform> transform = nullptr;

	Matrix ViewMatrix;
	Matrix ProjectMatrix;
	Matrix OrthoMatrix;
	Vector3 position = Vector3(0.f, 0.f, 0.f);
};

