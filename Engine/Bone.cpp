#include "pch.h"
#include "Bone.h"

Bone::Bone()
{
}

Bone::~Bone()
{
}

HRESULT Bone::Initialize(int _index, wstring _name, int _parentIndex, Matrix _transform)
{
	index = _index;
	name = _name;
	parentIndex = _parentIndex;
	transform = _transform;

	return S_OK;
}

shared_ptr<Bone> Bone::Create(int _index, wstring _name, int _parentIndex, Matrix _transform)
{
	shared_ptr<Bone> Instance(new Bone());

	if (FAILED(Instance->Initialize(_index, _name, _parentIndex, _transform)))
	{
		MSG_BOX("Failed to create Bone.");
		return nullptr;
	}
	return Instance;
}
