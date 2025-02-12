#include "Core/CorePCH.hpp"
#include "Core/Timer.hpp"

namespace CTMRenderer::Timer
{
	Timer::Timer() noexcept
	{
		LARGE_INTEGER frequency = {};

		QueryPerformanceFrequency(&frequency);
		m_CountFrequency = double(frequency.QuadPart);

		Reset();
	}

	void Timer::Reset() noexcept
	{
		QueryPerformanceCounter(&m_StartTime);
	}

	double Timer::ElapsedMillis() noexcept
	{
		QueryPerformanceCounter(&m_CurrentTime);
		return (static_cast<double>(m_CurrentTime.QuadPart - m_StartTime.QuadPart) / m_CountFrequency) * 1000;
	}
}