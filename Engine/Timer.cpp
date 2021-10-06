#include "pch.h"
#include "Timer.h"

Timer::Timer()
{
}

Timer::Timer(const Timer & Rhs)
{
}

Timer::~Timer()
{
}

HRESULT Timer::InitTimer()
{
	// QueryPerformanceCounter : 현재 cpu가 카운팅한 숫자를 얻어온다.
	QueryPerformanceCounter(&FrameTime);
	QueryPerformanceCounter(&FixTime);
	QueryPerformanceCounter(&LastTime);

	// QueryPerformanceFrequency : cpu가 1초에 카운팅할 수 있는 최대 숫자를 얻어온다.
	QueryPerformanceFrequency(&CpuTick);

	return S_OK;
}

float Timer::ComputeTimeDelta()
{
	QueryPerformanceCounter(&FrameTime);

	TimeDelta = float(FrameTime.QuadPart - FixTime.QuadPart) / float(CpuTick.QuadPart);

	FixTime.QuadPart = FrameTime.QuadPart;

	// 1초에 한번씩
	if ((FrameTime.QuadPart - LastTime.QuadPart) > CpuTick.QuadPart)
	{
		QueryPerformanceFrequency(&CpuTick);
		LastTime = FrameTime;
	}

	return float(TimeDelta);
}

Timer* Timer::Create()
{
	Timer* Instance = new Timer();

	if (nullptr == Instance || FAILED(Instance->InitTimer()))
	{
		MSG_BOX("Failed to create Timer.");
		return nullptr;
	}

	return Instance;
}

