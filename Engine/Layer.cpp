#include "pch.h"
#include "Layer.h"

int Layer::Update(float _timeDelta)
{
	return 0;
}

void Layer::Render()
{
}

HRESULT Layer::Initialize()
{
	return S_OK;
}

Layer * Layer::Create()
{
	auto Instance = new Layer();
	if (FAILED(Instance->Initialize()))
	{
		MSG_BOX("Failed to create Layer.");
		return nullptr;
	}

	return Instance;
}
