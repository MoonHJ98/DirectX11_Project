#include "pch.h"
#include "Light.h"
#include "GraphicDevice.h"
#include "Shader.h"
#include "Transform.h"
#include "RectangleBuffer.h"

Light::Light()
{
}

Light::Light(const Light & Rhs)
{
}

Light::~Light()
{
}


void Light::Render()
{
	LightBufferType buffertype;
	buffertype.DiffuseColor = LightInfo.Diffuse;
	buffertype.AmbientColor = LightInfo.Ambient;
	buffertype.SpecularColor = LightInfo.Specular;
	buffertype.SpecularPower = LightInfo.SpecularPower;
	buffertype.Direction = LightInfo.Direction;

	LightBuffer->SetData(Graphic->GetDeviceContext(), buffertype);
	auto buffer = LightBuffer->GetBuffer();
	Graphic->GetDeviceContext()->PSSetConstantBuffers(0, 1, &buffer);


	transform->Update(true);

	rb->Render();
}

HRESULT Light::Initialize(LIGHTDESC _LightInfo)
{
	LightInfo = _LightInfo;
	Graphic = GraphicDevice::GetInstance();
	shared_ptr<ConstantBuffer<LightBufferType>> temp(new ConstantBuffer<LightBufferType>());
	LightBuffer = temp;
	LightBuffer->Create(Graphic->GetDevice());

	rb = RectangleBuffer::Create();
	transform = Transform::Create(Transform::TRANSDESC());
	transform->SetState(Transform::POSITION, Vector3(0.f, 0.f, 0.1f));

	UINT viewportNum = 1;
	D3D11_VIEWPORT viewport;
	Graphic->GetDeviceContext()->RSGetViewports(&viewportNum, &viewport);

	transform->SetScale(Vector3(viewport.Width/2.f, viewport.Height/2.f, 1.f));
	return S_OK;
}

shared_ptr<Light> Light::Create(LIGHTDESC _LightInfo)
{
	shared_ptr<Light> Instance(new Light());
	if (FAILED(Instance->Initialize(_LightInfo)))
	{
		MSG_BOX("Failed to create Light.");
		return nullptr;
	}
	return Instance;
}
