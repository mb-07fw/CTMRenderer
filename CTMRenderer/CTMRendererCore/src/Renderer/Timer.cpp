#include "Core/CorePCH.hpp"
#include "CTMRenderer/Timer.hpp"

namespace CTMRenderer::Timer
{
	Timer::Timer() noexcept
	{
        Start();
	}

    void Timer::Start() noexcept
    {
        m_StartTime = std::chrono::high_resolution_clock::now();

    }

    void Timer::Reset() noexcept
    {
        m_StartTime = std::chrono::high_resolution_clock::now();
    }

    double Timer::ElapsedMillis() const noexcept
    {
        auto now = std::chrono::high_resolution_clock::now();
        return std::chrono::duration<double, std::milli>(now - m_StartTime).count();
    }
}