#pragma once

#include <chrono>

namespace CTMRenderer::Timer
{
	// TODO: Store time points as doubles, so there can be actual stopping and resetting functionality.
	class Timer
	{
		using TimePoint = std::chrono::high_resolution_clock::time_point;
	public:
		Timer() noexcept;
	public:
		void Start() noexcept;
		void Reset() noexcept;
		double ElapsedMillis() const noexcept;
	private:
		TimePoint m_StartTime;
	};
}