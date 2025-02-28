#pragma once

#include <chrono>

namespace CTMRenderer::Utils
{
	// TODO: Store time points as doubles, so there can be actual stopping and resetting functionality.
	class CTMTimer
	{
		using TimePoint = std::chrono::high_resolution_clock::time_point;
	public:
		CTMTimer() noexcept;
	public:
		void Start() noexcept;
		void Reset() noexcept;
		double ElapsedMillis() const noexcept;
	private:
		TimePoint m_StartTime;
	};
}