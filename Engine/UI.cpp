#include "pch.h"
#include "UI.h"
#include "GraphicDevice.h"
#include "RectangleBuffer.h"
#include "Renderer.h"
#include "Material.h"

UI::UI()
{
}

UI::UI(const UI & Rhs)
{
}

UI::~UI()
{
}

int UI::Update(float _TimeDelta)
{
	UpdateComponent(_TimeDelta);
	Renderer::GetInstance()->AddRenderGroup(Renderer::RENDER_UI, shared_from_this());

	return 0;
}

void UI::Render()
{
	
	RenderComponent();
}

void UI::OnContact()
{
}

void UI::OnTrigger()
{
}

HRESULT UI::Initialize()
{
	//TEXTUREDESC desc;
	//desc.Path = L"../Resources/test.tga";

	MATERIALDESC desc;
	desc.DiffuseMap = L"../Resources/test.tga";
	material = Material::Create(desc);

	rectBuffer = RectangleBuffer::Create();

	transform = Transform::Create(Transform::TRANSDESC());

	transform->SetState(Transform::POSITION, Vector3(0.f, 0.f, 0.1f));
	transform->SetScale(Vector3(50.f, 50.f, 1.f));

	D3D11_INPUT_ELEMENT_DESC InputLayout[] =
	{
		{ "POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 0, D3D11_INPUT_PER_VERTEX_DATA, 0 },
		{ "TEXCOORD", 0, DXGI_FORMAT_R32G32_FLOAT, 0, D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_VERTEX_DATA, 0 },

	};

	shader = Shader::Create(InputLayout, sizeof(InputLayout), L"../Engine/TextureVS.hlsl", L"../Engine/TexturePS.hlsl");

	components[ComponentType::TRANSFORM] = transform;
	components[ComponentType::SHADER] = shader;
	components[ComponentType::BUFFER] = rectBuffer;
	components[ComponentType::MATERIAL] = material;


	return S_OK;
}

shared_ptr<UI> UI::Create()
{
	shared_ptr<UI> Instance(new UI());
	if (FAILED(Instance->Initialize()))
	{
		MSG_BOX("Failed to create UI.");
		return nullptr;
	}
	return Instance;
}
