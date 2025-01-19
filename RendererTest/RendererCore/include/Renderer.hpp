#pragma once

#include <thread> // std::thread
#include <atomic> // std::atomic_bool
#include <mutex>  // std::mutex
#include <condition_variable> // std::condition_variable

#include "Window.hpp"
#include "Event/EventManager.hpp"
#include "Event/EventManagerInterface.hpp"

namespace Renderer
{
	class Renderer
	{
	public:
		Renderer();
		Renderer(const Renderer&) = delete;
		Renderer(Renderer&&) = delete;
		Renderer& operator=(const Renderer&) = delete;
		Renderer& operator=(Renderer&&) = delete;
	public:
		void Start();
		void JoinForShutdown();
	private:
		void OnStart();
		void OnEnd();
		void OnEvent();
		void EventLoop();
	private:
		Event::EventManager m_EventManager;
		Event::EventManagerInterface m_EventManagerInterface;
		Window::Window m_Window;
		std::thread m_EventThread;
		std::atomic_bool m_ShouldRun;
		std::atomic_bool m_EventLoopStarted;
		std::atomic_bool m_RendererStarted;
		std::mutex m_RendererMutex;
		std::condition_variable m_RendererCV;
	};
}