#include "Core/CorePCH.hpp"
#include "Core/CoreMacros.hpp"
#include "Core/CoreUtility.hpp"
#include "CTMRenderer/DirectX/DXRenderer.hpp"

namespace CTMRenderer::CTMDirectX
{
	DXRenderer::DXRenderer(const unsigned int targetFPS)
		: ICTMRenderer(), m_Settings(targetFPS), m_Window(m_Settings, m_EventSystem.Dispatcher()),
		  m_Graphics(m_Settings, m_Window.ClientArea(), m_Window.Mouse())
	{
	}

	#pragma region Public API
	void DXRenderer::Start() noexcept
	{
		m_ShouldRun.store(true, std::memory_order_release);
		m_EventThread = std::thread(&DXRenderer::EventLoop, this);

		// Wait for the event loop to start.
		std::unique_lock<std::mutex> lock(m_RendererMutex);
		m_RendererCV.wait(lock, [this] { return m_EventLoopStarted.load(std::memory_order_acquire); });

		m_EventSystem.Dispatcher().QueueEvent<Event::StartEvent>(1738u); // ayy

		//DEBUG_PRINT("Initialized renderer.\n");
	}

	void DXRenderer::JoinForShutdown() noexcept
	{
		m_EventThread.join();
	}
	#pragma endregion

	#pragma region Private Functions
	void DXRenderer::OnStart(const Event::StartEvent* pStartEvent) noexcept
	{
		RUNTIME_ASSERT(pStartEvent != nullptr, "Start event is nullptr. How TF did this happen?\n");
		RUNTIME_ASSERT(m_RendererStarted.load(std::memory_order_acquire) == false, "Renderer has already started.\n");

		DEBUG_PRINT("Start args : " << pStartEvent->PlaceholderArgs() << '\n');

		m_Window.Start();
		m_Graphics.Init(m_Window.Handle());

		m_RendererStarted.store(true, std::memory_order_release);

		DEBUG_PRINT("Renderer started.\n");
	}

	void DXRenderer::OnEnd(const Event::EndEvent* pEndEvent) noexcept
	{
		RUNTIME_ASSERT(pEndEvent != nullptr, "End event is nullptr. How TF did this happen?\n");
		RUNTIME_ASSERT(m_EventLoopStarted.load(std::memory_order_acquire) == true, "Event loop hasn't started.\n");
		RUNTIME_ASSERT(m_RendererStarted.load(std::memory_order_acquire) == true, "Renderer hasn't started.\n");
		RUNTIME_ASSERT(m_ShouldRun.load(std::memory_order_acquire) == true, "Renderer has already shutdown.\n");

		DEBUG_PRINT("End args : " << pEndEvent->PlaceholderArgs() << '\n');

		m_ShouldRun.store(false, std::memory_order_release);

		DEBUG_PRINT("Renderer ended.\n");
	}

	void DXRenderer::EventLoop() noexcept
	{
		// The passed onNotifyFunc will be called when an event is dispatched.
		Event::GenericListener<Event::GenericEventType::CTM_ANY> eventListenerAny(
			std::bind(&DXRenderer::HandleEvent, this, std::placeholders::_1)
		);

		Event::EventDispatcher& eventDispatcher = m_EventSystem.Dispatcher();
		eventDispatcher.Subscribe(&eventListenerAny);

		{
			std::lock_guard<std::mutex> lock(m_RendererMutex);
			m_EventLoopStarted.store(true, std::memory_order_release);

			m_RendererCV.notify_one();
		}

		DEBUG_PRINT("Renderer event loop started.\n");

		const double targetFrameDuration = 1000.0 / m_Settings.TargetFPS;
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

			actualFrameDuration = m_Timer.ElapsedMillis() - frameStartTime;
			remainingFrameTime = Utility::MinDB(targetFrameDuration - actualFrameDuration, 0);
			RUNTIME_ASSERT(actualFrameDuration >= 0, "YOU DID SOMETHING WRONG YOU IDIOT!!!\n");
			RUNTIME_ASSERT(remainingFrameTime >= 0, "YOU DID SOMETHING WRONG YOU IDIOT!!!\n");

			Sleep(static_cast<DWORD>(remainingFrameTime));
		}

		DEBUG_PRINT("Event loop end.\n");
	}

	void DXRenderer::HandleEvent(Event::IEvent* pEvent) noexcept
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
		case Event::GenericEventType::CTM_FRAME_EVENT:
			if (pEvent->ConcreteType() == Event::ConcreteEventType::CTM_FRAME_CLEAR_FRAME_EVENT)
				DEBUG_PRINT("Received an ClearFrameEvent.\n");
			else
				DEBUG_PRINT("Recieved a FrameEvent.\n");
			break;
		default:
			RUNTIME_ASSERT(false, "Received event has an unknown GenericEventType.\n");
		}
	}

	void DXRenderer::HandleStateEvent(Event::IEvent* pEvent) noexcept
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
			RUNTIME_ASSERT(false, "State event wasn't handled.\n");
		}
	}

	void DXRenderer::HandleMouseEvent(Event::IEvent* pEvent) noexcept
	{
		RUNTIME_ASSERT(pEvent->GenericType() == Event::GenericEventType::CTM_MOUSE_EVENT, "The provided event wasn't a CTM_STATE_EVENT.\n");

		switch (pEvent->ConcreteType())
		{
		case Event::ConcreteEventType::CTM_MOUSE_MOVE_EVENT:
		{
			Event::MouseMoveEvent* pMMoveEvent = Event::MouseMoveEvent::Cast(pEvent);
			m_Window.Mouse().SetPos(pMMoveEvent->PosX(), pMMoveEvent->PosY());
			m_Window.SetTitle(std::wstring(L'(' + std::to_wstring(pMMoveEvent->PosX()) + L", " + std::to_wstring(pMMoveEvent->PosY()) + L')'));
			break;
		}
		default:
			RUNTIME_ASSERT(false, "Mouse event wasn't handled.\n");
		}
	}
	#pragma endregion
}
