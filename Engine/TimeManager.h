#pragma once

#include "Timer.h"

class Timer;

class TimeManager
{
	SINGLETON(TimeManager)

public:
	~TimeManager();

private:
	typedef map<wstring, Timer*>	MAPTIMERS;

public:
	float							GetTimeDelta(wstring _TimerTag);
public:
	HRESULT							AddTimer(wstring TimerTag);
	float							ComputeTimer(wstring _TimerTag);
	Timer*							FindTimer(wstring _TimerTag);

private:
	MAPTIMERS						Timers;
};

