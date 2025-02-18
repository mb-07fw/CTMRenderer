#pragma once

#include <thread> // std::thread
#include <atomic> // std::atomic_bool
#include <mutex>  // std::mutex
#include <condition_variable> // std::condition_variable

#include "Event/EventSystem.hpp"
#include "CTMRenderer/Timer.hpp"

namespace CTMRenderer
{
	enum class RendererType
	{
		CTM_DIRECTX
	};

	class IRenderer
	{
	public:
		IRenderer() = default;
		virtual ~IRenderer() = default;
	public:
		virtual void Start() noexcept = 0;
		virtual void JoinForShutdown() noexcept = 0;
	protected:
		Event::EventSystem m_EventSystem;
		Timer::Timer m_Timer;
		std::thread m_EventThread;
		std::mutex m_RendererMutex;
		std::condition_variable m_RendererCV;
		std::atomic_bool m_ShouldRun = false;
		std::atomic_bool m_EventLoopStarted = false;
		std::atomic_bool m_RendererStarted = false;
	};
}