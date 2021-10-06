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
	// QueryPerformanceCounter : ���� cpu�� ī������ ���ڸ� ���´�.
	QueryPerformanceCounter(&FrameTime);
	QueryPerformanceCounter(&FixTime);
	QueryPerformanceCounter(&LastTime);

	// QueryPerformanceFrequency : cpu�� 1�ʿ� ī������ �� �ִ� �ִ� ���ڸ� ���´�.
	QueryPerformanceFrequency(&CpuTick);

	return S_OK;
}

float Timer::ComputeTimeDelta()
{
	QueryPerformanceCounter(&FrameTime);

	TimeDelta = float(FrameTime.QuadPart - FixTime.QuadPart) / float(CpuTick.QuadPart);

	FixTime.QuadPart = FrameTime.QuadPart;

	// 1�ʿ� �ѹ���
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

