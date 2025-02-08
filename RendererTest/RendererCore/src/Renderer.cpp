#include "Core/CorePCH.hpp"
#include "Core/CoreMacros.hpp"
#include "Core/CoreUtility.hpp"
#include "Renderer.hpp"

namespace CTMRenderer
{
	CTMRenderer::CTMRenderer(const unsigned int targetFPS)
		: m_EventSystem(), m_Window(m_EventSystem.Dispatcher(), targetFPS), m_Timer(),
		  m_EventThread(), m_ShouldRun(true), m_EventLoopStarted(false),
		  m_RendererStarted(false), m_RendererMutex(), m_RendererCV(), m_TargetFPS(targetFPS)
	{
	}

	#pragma region Public API
	void CTMRenderer::Start()
	{
		m_EventThread = std::thread(&CTMRenderer::EventLoop, this);

		// Wait for the event loop to start.
		std::unique_lock<std::mutex> lock(m_RendererMutex);
		m_RendererCV.wait(lock, [this] { return m_EventLoopStarted.load(std::memory_order_acquire); });

		// TODO: Figure out way to have events be dispatched on the Renderer's thread
		//		 to prevent creation of the window on the main thread. (Dispatching queue)
		m_EventSystem.Dispatcher().QueueEvent<Event::StartEvent>(1738u);

		//DEBUG_PRINT("Initialized renderer.\n");
	}

	void CTMRenderer::JoinForShutdown()
	{
		m_EventThread.join();
	}
	#pragma endregion

	#pragma region Private Functions
	void CTMRenderer::OnStart(const Event::StartEvent* pStartEvent) noexcept
	{
		RUNTIME_ASSERT(pStartEvent != nullptr, "Start event is nullptr. How TF did this happen?\n");
		RUNTIME_ASSERT(m_RendererStarted.load(std::memory_order_acquire) == false, "Renderer has already started.\n");

		DEBUG_PRINT("Start args : " << pStartEvent->PlaceholderArgs() << '\n');

		m_Window.Start();

		m_RendererStarted.store(true, std::memory_order_release);

		DEBUG_PRINT("Renderer started.\n");
	}

	void CTMRenderer::OnEnd(const Event::EndEvent* pEndEvent) noexcept
	{
		RUNTIME_ASSERT(pEndEvent != nullptr, "End event is nullptr. How TF did this happen?\n");
		RUNTIME_ASSERT(m_EventLoopStarted.load(std::memory_order_acquire) == true, "Event loop hasn't started.\n");
		RUNTIME_ASSERT(m_RendererStarted.load(std::memory_order_acquire) == true, "Renderer hasn't started.\n");
		RUNTIME_ASSERT(m_ShouldRun.load(std::memory_order_acquire) == true, "Renderer has already shutdown.\n");

		DEBUG_PRINT("End args : " << pEndEvent->PlaceholderArgs() << '\n');

		m_ShouldRun.store(false, std::memory_order_release);

		DEBUG_PRINT("Renderer ended.\n");
	}

	void CTMRenderer::EventLoop() noexcept
	{
		// The passed onNotifyFunc will be called when an event is dispatched.
		Event::GenericListener<Event::GenericEventType::CTM_ANY> eventListenerAny(
			std::bind(&CTMRenderer::HandleEvent, this, std::placeholders::_1)
		);

		Event::EventDispatcher& eventDispatcher = m_EventSystem.Dispatcher();
		eventDispatcher.Subscribe(&eventListenerAny);

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

			if (eventDispatcher.IsEventQueued())
				eventDispatcher.DispatchQueued();

			if (m_Window.IsRunning())
				m_Window.DoFrame();

			actualFrameDuration = m_Timer.ElapsedMillis() - frameStartTime;
			remainingFrameTime = Utility::MinDB(targetFrameDuration - actualFrameDuration, 0);
			RUNTIME_ASSERT(actualFrameDuration >= 0, "YOU DID SOMETHING WRONG YOU IDIOT!!!\n");
			RUNTIME_ASSERT(remainingFrameTime >= 0, "YOU DID SOMETHING WRONG YOU IDIOT!!!\n");

			Sleep(static_cast<DWORD>(remainingFrameTime));
		}

		DEBUG_PRINT("Event loop end.\n");
	}

	void CTMRenderer::HandleEvent(Event::IEvent* pEvent) noexcept
	{
		RUNTIME_ASSERT(pEvent != nullptr, "Event received is nullptr.\n");

		Event::GenericEventType genericType = pEvent->GenericType();

		switch (genericType)
		{
		case Event::GenericEventType::CTM_ANY:
			RUNTIME_ASSERT(false, "No events should have CTM_ANY as their GenericEventType, as it is a marker type used for event listening.\n");
			break;
		case Event::GenericEventType::CTM_STATE_EVENT:
			HandleStateEvent(pEvent);
			break;
		case Event::GenericEventType::CTM_MOUSE_EVENT:
			HandleMouseEvent(pEvent);
			break;
		default: break;
		}
	}

	void CTMRenderer::HandleStateEvent(Event::IEvent* pEvent) noexcept
	{
		RUNTIME_ASSERT(pEvent->GenericType() == Event::GenericEventType::CTM_STATE_EVENT, "The provided event wasn't a CTM_STATE_EVENT.\n");

		switch (pEvent->ConcreteType())
		{
		case Event::ConcreteEventType::CTM_STATE_START_EVENT:
			OnStart(Event::StartEvent::Cast(pEvent));
			break;
		case Event::ConcreteEventType::CTM_STATE_END_EVENT:
			OnEnd(Event::EndEvent::Cast(pEvent));
			break;
		default:
			RUNTIME_ASSERT(false, "Event wasn't handled.\n");
		}
	}

	void CTMRenderer::HandleMouseEvent(Event::IEvent* pEvent) noexcept
	{
		RUNTIME_ASSERT(pEvent->GenericType() == Event::GenericEventType::CTM_MOUSE_EVENT, "The provided event wasn't a CTM_STATE_EVENT.\n");

		switch (pEvent->ConcreteType())
		{
		case Event::ConcreteEventType::CTM_MOUSE_MOVE_EVENT:
		{
			Event::MouseMoveEvent* pMMoveEvent = Event::MouseMoveEvent::Cast(pEvent);
			m_Window.SetTitle(std::wstring(L'(' + std::to_wstring(pMMoveEvent->PosX()) + L", " + std::to_wstring(pMMoveEvent->PosY()) + L')'));
			break;
		}
		}
	}
	#pragma endregion
}
