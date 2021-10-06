#include "pch.h"
#include "TimeManager.h"
#include "Timer.h"

TimeManager::~TimeManager()
{
	for (auto& Pair : Timers)
	{
		delete Pair.second;
		Pair.second = nullptr;
	}
	Timers.clear();
}

float TimeManager::GetTimeDelta(wstring _TimerTag)
{
	Timer* Timer = FindTimer(_TimerTag);

	if (nullptr == Timer)
		return 0.0;

	return Timer->GetTimeDelta();
}

HRESULT TimeManager::AddTimer(wstring TimerTag)
{
	Timer* Timer = FindTimer(TimerTag);

	if (nullptr != Timer)
		return E_FAIL;

	Timer = Timer::Create();
	if (nullptr == Timer)
		return E_FAIL;

	Timers.insert(MAPTIMERS::value_type(TimerTag, Timer));


	return S_OK;
}

float TimeManager::ComputeTimer(wstring _TimerTag)
{
	Timer* Timer = FindTimer(_TimerTag);

	if (nullptr == Timer)
		return 0.f;

	return Timer->ComputeTimeDelta();
}

Timer* TimeManager::FindTimer(wstring _TimerTag)
{
	auto iter = Timers.find(_TimerTag);

	if (iter == Timers.end())
		return nullptr;

	return iter->second;
}
