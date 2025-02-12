#pragma once

#include <thread> // std::thread
#include <atomic> // std::atomic_bool
#include <mutex>  // std::mutex
#include <condition_variable> // std::condition_variable

#include "DirectX/Window/DXWindow.hpp"
#include "Event/EventSystem.hpp"
#include "Core/Timer.hpp"

namespace CTMRenderer
{
	class CTMRenderer
	{
	public:
		CTMRenderer(const unsigned int targetFPS);
		CTMRenderer(const CTMRenderer&) = delete;
		CTMRenderer(CTMRenderer&&) = delete;
		CTMRenderer& operator=(const CTMRenderer&) = delete;
		CTMRenderer& operator=(CTMRenderer&&) = delete;
		~CTMRenderer() = default;
	public:
		void Start();
		void JoinForShutdown();
	private:
		void OnStart(const Event::StartEvent* pEvent) noexcept;
		void OnEnd(const Event::EndEvent* pEvent) noexcept;
		void EventLoop() noexcept;
		void HandleEvent(Event::IEvent* pEvent) noexcept;
		void HandleStateEvent(Event::IEvent* pEvent) noexcept;
		void HandleMouseEvent(Event::IEvent* pEvent) noexcept;
	private:
		Event::EventSystem m_EventSystem;
		CTMDirectX::Window::Window m_Window;
		Timer::Timer m_Timer;
		std::thread m_EventThread;
		std::atomic_bool m_ShouldRun;
		std::atomic_bool m_EventLoopStarted;
		std::atomic_bool m_RendererStarted;
		std::mutex m_RendererMutex;
		std::condition_variable m_RendererCV;
		unsigned int m_TargetFPS;
	};
}