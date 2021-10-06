#pragma once

#include "Includes.h"

class Timer
{
private:
							Timer();
							Timer(const Timer& Rhs);

public:
							~Timer();

public:
	inline float				GetTimeDelta() const { return TimeDelta; }

public:
	HRESULT						InitTimer();
	float						ComputeTimeDelta();

private:
	float						TimeDelta = 0.f;

private:
	LARGE_INTEGER				FrameTime;
	LARGE_INTEGER				FixTime;
	LARGE_INTEGER				LastTime;
	LARGE_INTEGER				CpuTick;

public:
	static Timer*			Create();

};

