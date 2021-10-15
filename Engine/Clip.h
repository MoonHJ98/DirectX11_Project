#pragma once

#include "Includes.h"

class Clip
{
private:
	Clip();
	Clip(const Clip& Rhs);

public:
	~Clip();

public:
	wstring						GetName()       { return name; }
	float						GetDuration()   { return duration; }
	float						GetFrameRate()  { return frameRate; }
	UINT						GetFrameCount() { return frameCount; }

	shared_ptr<ModelKeyframe>	GetKeyframe(wstring boneName);
	void						SetKeyframe(wstring _boneName, shared_ptr<ModelKeyframe> _keyframe);

private:
	HRESULT						Initialize(wstring _name, float _duration, float _frameRate, UINT _frameCount);

public:
	static shared_ptr<Clip>		Create(wstring _name, float _duration, float _frameRate, UINT _frameCount);
private:
	wstring name = L"";
	float duration = 0.f;
	float frameRate = 0.f;
	UINT frameCount = 0;

	// unordered_map : �ؽ��ڵ� ���. �ؽ���ȯ�� �ȴٸ� map���� �˻��� ����. ���� �����͸� ã������ map���� ��ȣ.
	unordered_map<wstring, shared_ptr<ModelKeyframe>> keyframeMap;
};

