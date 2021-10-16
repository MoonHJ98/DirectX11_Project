#include "pch.h"
#include "Clip.h"

Clip::Clip()
{
}

Clip::Clip(const Clip & Rhs)
{
}

Clip::~Clip()
{
}

shared_ptr<ModelKeyframe> Clip::GetKeyframe(wstring boneName)
{
	if (keyframeMap.count(boneName) < 1)
		return nullptr;

	return keyframeMap[boneName];
}

void Clip::SetKeyframe(wstring _boneName, shared_ptr<ModelKeyframe> _keyframe)
{
	keyframeMap[_boneName] = _keyframe;

}

HRESULT Clip::Initialize(wstring _name, float _duration, float _frameRate, UINT _frameCount)
{
	name = _name;
	duration = _duration;
	frameRate = _frameRate;
	frameCount = _frameCount;

	return S_OK;
}

shared_ptr<Clip> Clip::Create(wstring _name, float _duration, float _frameRate, UINT _frameCount)
{
	shared_ptr<Clip> Instance(new Clip());
	if (Instance->Initialize(_name, _duration, _frameRate, _frameCount))
	{
		MSG_BOX("Failed to create Clip.");
		return nullptr;
	}
	return Instance;
}
