#pragma once

#include "Core/CorePCH.hpp"

namespace Renderer::Timer
{
	class Timer
	{
	public:
		Timer() noexcept;
	public:
		void Reset() noexcept;
		double ElapsedMillis() noexcept;
	private:
		LARGE_INTEGER m_StartTime;
		LARGE_INTEGER m_CurrentTime;
		double m_CountFrequency;
	};
}