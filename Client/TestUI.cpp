#include "pch.h"
#include "TestUI.h"
#include "Shader.h"
#include "Transform.h"
#include "RectangleBuffer.h"
#include "GraphicDevice.h"

TestUI::TestUI()
{

}

TestUI::~TestUI()
{
}

HRESULT TestUI::Initialize()
{
	Graphic = GraphicDevice::GetInstance();
	transform = Transform::Create(Transform::TRANSDESC());
	rb = RectangleBuffer::Create();

	D3D11_INPUT_ELEMENT_DESC InputLayout[] =
	{
		{ "POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 0, D3D11_INPUT_PER_VERTEX_DATA, 0 },
		{ "TEXCOORD", 0, DXGI_FORMAT_R32G32_FLOAT, 0, D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_VERTEX_DATA, 0 },

	};

	shader = Shader::Create(InputLayout, sizeof(InputLayout), L"../Engine/TextureVS.hlsl", L"../Engine/TexturePS.hlsl");
	return S_OK;
}

int TestUI::Update(float _TimeDelta)
{

	return 0;
}

void TestUI::Render()
{
	Graphic->TurnZBufferOff();
	transform->Update(true, true);
	shader->Render();
	rb->Render();
	//Graphic->GetDeviceContext()->PSSetShaderResources(0, 1, nullptr);


	Graphic->GetDeviceContext()->DrawIndexed(/*debugWindow*/rb->GetIndexCount(), 0, 0);
	Graphic->TurnZBufferOn();

}

shared_ptr<TestUI> TestUI::Create()
{
	shared_ptr<TestUI> Instance(new TestUI());
	if (FAILED(Instance->Initialize()))
	{
		MSG_BOX("Failed to create TestUI.");
		return nullptr;
	}
	return Instance;
}
