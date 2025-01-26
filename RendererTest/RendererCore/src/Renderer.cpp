#include "Core/CorePCH.hpp"
#include "Core/CoreMacros.hpp"
#include "Core/CoreUtility.hpp"
#include "Renderer.hpp"

namespace Renderer
{
	Renderer::Renderer(const unsigned int targetFPS)
		: m_EventManager(), m_Window(m_EventManager, targetFPS), m_Timer(),
		  m_EventThread(), m_ShouldRun(true), m_EventLoopStarted(false),
		  m_RendererStarted(false), m_RendererMutex(), m_RendererCV(), m_TargetFPS(targetFPS)
	{
	}

	#pragma region Public API
	void Renderer::Start()
	{
		m_EventThread = std::thread(&Renderer::EventLoop, this);

		// Wait for the event loop to start.
		std::unique_lock<std::mutex> lock(m_RendererMutex);
		m_RendererCV.wait(lock, [this] { return m_EventLoopStarted.load(std::memory_order_acquire); });

		m_EventManager.BroadcastEventSafe<Event::RendererStartEvent, Event::ConcreteEventType::RENDERER_START>(69u);
		DEBUG_PRINT("Broadcasted start.\n");

		DEBUG_PRINT("Initialized renderer.\n");
	}

	void Renderer::JoinForShutdown()
	{
		m_EventThread.join();
	}
	#pragma endregion

	#pragma region Private Functions
	void Renderer::OnStart(const Event::RendererStartEvent* pStartEvent) noexcept
	{
		RUNTIME_ASSERT(pStartEvent != nullptr, "Start event is nullptr. How TF did this happen?\n");
		RUNTIME_ASSERT(m_RendererStarted.load(std::memory_order_acquire) == false, "Renderer has already started.\n");

		DEBUG_PRINT("Start args : " << pStartEvent->Value() << '\n');

		m_Window.Start();

		m_RendererStarted.store(true, std::memory_order_release);

		DEBUG_PRINT("Renderer started.\n");
	}

	void Renderer::OnEnd(const Event::RendererEndEvent* pEndEvent) noexcept
	{
		RUNTIME_ASSERT(pEndEvent != nullptr, "End event is nullptr. How TF did this happen?\n");
		RUNTIME_ASSERT(m_EventLoopStarted.load(std::memory_order_acquire) == true, "Event loop hasn't started.\n");
		RUNTIME_ASSERT(m_RendererStarted.load(std::memory_order_acquire) == true, "Renderer hasn't started.\n");
		RUNTIME_ASSERT(m_ShouldRun.load(std::memory_order_acquire) == true, "Renderer has already shutdown.\n");

		DEBUG_PRINT("End args : " << pEndEvent->Value() << '\n');

		m_ShouldRun.store(false, std::memory_order_release);

		DEBUG_PRINT("Renderer ended.\n");
	}

	void Renderer::EventLoop() noexcept
	{
		const std::weak_ptr<Event::EventListener> weakEventListener = m_EventManager.GetActiveListenerSafe(Event::ListenType::LISTEN_RENDERER_STATE);
		RUNTIME_ASSERT(weakEventListener.expired() == false, "Retrieved weak ptr to listener is expired.\n");

		std::shared_ptr<Event::EventListener> eventListener = weakEventListener.lock();
		RUNTIME_ASSERT(eventListener->IsActive() == true, "State listener isn't active.\n");

		{
			std::lock_guard<std::mutex> lock(m_RendererMutex);
			m_EventLoopStarted.store(true, std::memory_order_release);

			m_RendererCV.notify_one();
		}

		DEBUG_PRINT("Renderer event loop started.\n");

		const double targetFrameDuration = 1000.0 / m_TargetFPS;
		double actualFrameDuration = 0.0;
		double frameStartTime = 0.0;
		double remainingFrameTime = 0.0;

		BOOL result;
		MSG msg;
		while (m_ShouldRun.load(std::memory_order_acquire))
		{
			frameStartTime = m_Timer.ElapsedMillis();

			if (m_RendererStarted.load(std::memory_order_acquire))
				m_Window.HandleMessages(result, msg);

			if (eventListener->IsNotified())
			{
				while (!eventListener->IsEventQueueEmpty())
					HandleEvent(eventListener->PopOldest());

				eventListener->ClearNotification();
			}

			if (m_Window.IsInitialized())
				m_Window.DoFrame();

			actualFrameDuration = m_Timer.ElapsedMillis() - frameStartTime;
			remainingFrameTime = Utility::MinDB(targetFrameDuration - actualFrameDuration, 0);
			RUNTIME_ASSERT(actualFrameDuration >= 0, "YOU DID SOMETHING WRONG YOU IDIOT!!!\n");
			RUNTIME_ASSERT(remainingFrameTime >= 0, "YOU DID SOMETHING WRONG YOU IDIOT!!!\n");

			Sleep(static_cast<DWORD>(remainingFrameTime));
		}

		DEBUG_PRINT("Event loop end.\n");
	}

	void Renderer::HandleEvent(const Event::Event* pEvent) noexcept
	{
		using namespace Event;
		
		RUNTIME_ASSERT(pEvent != nullptr, "Event received is nullptr.\n");

		AbstractEventType absType = pEvent->AbsType();

		switch (absType)
		{
		case AbstractEventType::RENDERER_STATE:
			RUNTIME_ASSERT(dynamic_cast<const RendererStateEvent*>(pEvent) != nullptr, "Types do not match.\n");
			HandleStateEvent(static_cast<const RendererStateEvent*>(pEvent));
			break;
		}
	}

	void Renderer::HandleStateEvent(const Event::RendererStateEvent* pStateEvent) noexcept
	{
		using namespace Event;

		switch (pStateEvent->StateType())
		{
		case RendererStateEventType::START:
			RUNTIME_ASSERT(dynamic_cast<const RendererStartEvent*>(pStateEvent) != nullptr, "Types do not match.\n");
			OnStart(static_cast<const RendererStartEvent*>(pStateEvent));
			break;
		case RendererStateEventType::END:
			RUNTIME_ASSERT(dynamic_cast<const RendererEndEvent*>(pStateEvent) != nullptr, "Types do not match.\n");
			OnEnd(static_cast<const RendererEndEvent*>(pStateEvent));
			break;
		}
	}
	#pragma endregion
}
