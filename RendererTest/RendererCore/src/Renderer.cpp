#include "Core/CorePCH.hpp"
#include "Core/CoreMacros.hpp"
#include "Core/CoreUtility.hpp"
#include "Renderer.hpp"

namespace Renderer
{
	Renderer::Renderer()
		: m_EventManager(), m_EventManagerInterface(m_EventManager), m_Window(m_EventManagerInterface), 
		  m_EventThread(), m_ShouldRun(true), m_EventLoopStarted(false),
		  m_RendererStarted(false), m_RendererMutex(), m_RendererCV()
	{
		DEBUG_PRINT("Initialized renderer.\n");
	}

	void Renderer::Start()
	{
		m_EventThread = std::thread(&Renderer::EventLoop, this);

		DEBUG_PRINT("Waiting for event loop start.\n");

		// Wait for the event loop to start.
		std::unique_lock<std::mutex> lock(m_RendererMutex);
		m_RendererCV.wait(lock, [this] { return m_EventLoopStarted.load(std::memory_order_acquire); });

		DEBUG_PRINT("Done waiting for event loop start.\n");

		m_EventManager.BroadcastEvent(Event::EventType::RENDERER_START);
		DEBUG_PRINT("Broadcasted start.\n");
	}

	void Renderer::EventLoop()
	{
		const std::weak_ptr<Event::EventListener> listener = m_EventManager.GetActiveListener(Event::ListenType::LISTEN_RENDERER_STATE);

		RUNTIME_ASSERT(listener.expired() == false, "Retrieved weak ptr to listener is expired.\n");

		std::shared_ptr<Event::EventListener> sharedListener = listener.lock();

		RUNTIME_ASSERT(sharedListener->IsActive() == true, "State listener isn't active.\n");

		{
			std::lock_guard<std::mutex> lock(m_RendererMutex);
			m_EventLoopStarted.store(true, std::memory_order_release);

			m_RendererCV.notify_one();
		}

		DEBUG_PRINT("Renderer event loop started.\n");

		while (m_ShouldRun.load(std::memory_order_acquire))
		{
			if (sharedListener->IsNotified())
			{
				while (!sharedListener->IsEventQueueEmpty())
				{
					DEBUG_PRINT("Event listener is notified.\n");

					const Event::Event* event = sharedListener->PopOldest();

					RUNTIME_ASSERT(event != nullptr, "Event received is nullptr.\n");

					Event::EventType eventType = event->Type();

					RUNTIME_ASSERT(eventType != Event::EventType::INVALID, "Received event is invalid.\n");

					if (eventType == Event::EventType::RENDERER_START)
						OnStart();
					else if (eventType == Event::EventType::RENDERER_END)
						OnEnd();
				}

				sharedListener->ClearNotification();
			}

			if (m_Window.IsInitialized())
				m_Window.DoFrame();

			Sleep(100);
		}

		DEBUG_PRINT("Event loop end.\n");
	}

	void Renderer::JoinForShutdown()
	{
		m_EventThread.join();
		m_Window.Join();
	}

	#pragma region Private
	void Renderer::OnStart()
	{
		RUNTIME_ASSERT(m_RendererStarted.load(std::memory_order_acquire) == false, "Renderer has already started.\n");

		m_Window.Start();

		m_RendererStarted.store(true, std::memory_order_release);

		DEBUG_PRINT("Renderer started.\n");
	}

	void Renderer::OnEnd()
	{
		RUNTIME_ASSERT(m_EventLoopStarted.load(std::memory_order_acquire) == true, "Event loop hasn't started.\n");
		RUNTIME_ASSERT(m_RendererStarted.load(std::memory_order_acquire) == true, "Renderer hasn't started.\n");
		RUNTIME_ASSERT(m_ShouldRun.load(std::memory_order_acquire) == true, "Renderer has already shutdown.\n");

		m_ShouldRun.store(false, std::memory_order_release);

		DEBUG_PRINT("Renderer ended.\n");
	}

	void Renderer::OnEvent()
	{

	}
	#pragma endregion
}