#include "Core/CorePCH.hpp"
#include "CTMRenderer/CTMTimer.hpp"

namespace CTMRenderer::Utils
{
	CTMTimer::CTMTimer() noexcept
	{
        Start();
	}

    void CTMTimer::Start() noexcept
    {
        m_StartTime = std::chrono::high_resolution_clock::now();

    }

    void CTMTimer::Reset() noexcept
    {
        m_StartTime = std::chrono::high_resolution_clock::now();
    }

    double CTMTimer::ElapsedMillis() const noexcept
    {
        auto now = std::chrono::high_resolution_clock::now();
        return std::chrono::duration<double, std::milli>(now - m_StartTime).count();
    }
}