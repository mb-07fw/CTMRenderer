#pragma once

#include <thread> // std::thread
#include <atomic> // std::atomic_bool
#include <mutex>  // std::mutex
#include <condition_variable> // std::condition_variable

#include "Window.hpp"
#include "Event/EventManager.hpp"
#include "Timer.hpp"

namespace Renderer
{
	class Renderer
	{
	public:
		Renderer(const unsigned int targetFPS);
		Renderer(const Renderer&) = delete;
		Renderer(Renderer&&) = delete;
		Renderer& operator=(const Renderer&) = delete;
		Renderer& operator=(Renderer&&) = delete;
		~Renderer() = default;
	public:
		void Start();
		void JoinForShutdown();
	private:
		void OnStart(const Event::RendererStartEvent* pStartEvent) noexcept;
		void OnEnd(const Event::RendererEndEvent* pEndEvent) noexcept;
		void EventLoop() noexcept;
		void HandleEvent(const Event::Event* pEvent) noexcept;
		void HandleStateEvent(const Event::RendererStateEvent* pStateEvent) noexcept;
	private:
		Event::EventManager m_EventManager;
		Window::Window m_Window;
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