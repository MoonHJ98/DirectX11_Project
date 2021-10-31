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

public:
	void Render();

private:
	HRESULT Initialize(LIGHTDESC _LightInfo);

public:
	static shared_ptr<Light> Create(LIGHTDESC _LightInfo);

private:
	shared_ptr<GraphicDevice> Graphic = nullptr;
	LIGHTDESC LightInfo;
	shared_ptr<ConstantBuffer<LightBufferType>> LightBuffer = nullptr;

	shared_ptr<RectangleBuffer> rb = nullptr;
	shared_ptr<Transform> transform = nullptr;
};

