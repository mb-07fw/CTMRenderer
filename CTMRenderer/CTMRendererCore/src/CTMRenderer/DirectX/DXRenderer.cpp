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

	#pragma region Private Functions
	std::thread DXRenderer::StartEventThread() noexcept
	{
		return std::thread(&DXRenderer::EventLoop, this);
	}

	void DXRenderer::OnStart(const Event::CTMStartEvent* pStartEvent) noexcept
	{
		RUNTIME_ASSERT(pStartEvent != nullptr, "Start event is nullptr.\n");
		RUNTIME_ASSERT(m_RendererStarted.load(std::memory_order_acquire) == false, "Renderer has already started.\n");

		DEBUG_PRINT("Start args : " << pStartEvent->PlaceholderArgs() << '\n');

		m_Window.Start();
		m_Graphics.Init(m_Window.Handle());

		m_RendererStarted.store(true, std::memory_order_release);

		DEBUG_PRINT("Renderer started.\n");
	}

	void DXRenderer::OnEnd(const Event::CTMEndEvent* pEndEvent) noexcept
	{
		RUNTIME_ASSERT(pEndEvent != nullptr, "End event is nullptr.\n");
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
		Event::CTMGenericListener<Event::CTMGenericEventType::CTM_ANY> eventListenerAny(
			std::bind(&DXRenderer::HandleEvent, this, std::placeholders::_1)
		);

		Event::CTMEventDispatcher& eventDispatcher = m_EventSystem.Dispatcher();
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
			{
				eventDispatcher.DispatchQueued();

				if (!m_ShouldRun.load(std::memory_order_acquire))
					return;
			}

			DrawQueued();

			actualFrameDuration = m_Timer.ElapsedMillis() - frameStartTime;
			remainingFrameTime = Utility::MinDB(targetFrameDuration - actualFrameDuration, 0);
			RUNTIME_ASSERT(actualFrameDuration >= 0, "YOU DID SOMETHING WRONG YOU IDIOT!!!\n");
			RUNTIME_ASSERT(remainingFrameTime >= 0, "YOU DID SOMETHING WRONG YOU IDIOT!!!\n");

			Sleep(static_cast<DWORD>(remainingFrameTime));
		}

		DEBUG_PRINT("Event loop end.\n");
	}

	void DXRenderer::DrawQueued() noexcept
	{
		if (!m_DrawQueue.RectQueue().empty())
		{
			DEBUG_PRINT("Queued rect found.\n");
			m_DrawQueue.RectQueue().clear();
		}
	}

	void DXRenderer::HandleEvent(Event::ICTMEvent* pEvent) noexcept
	{
		RUNTIME_ASSERT(pEvent != nullptr, "Event received is nullptr.\n");

		switch (pEvent->GenericType())
		{
		case Event::CTMGenericEventType::CTM_ANY:
			RUNTIME_ASSERT(false, "No events should have CTM_ANY as their GenericEventType, as it is a marker type used for event listening.\n");
			break;
		case Event::CTMGenericEventType::CTM_STATE_EVENT:
			HandleStateEvent(pEvent);
			break;
		case Event::CTMGenericEventType::CTM_MOUSE_EVENT:
			HandleMouseEvent(pEvent);
			break;
		case Event::CTMGenericEventType::CTM_FRAME_EVENT:
			HandleFrameEvent(pEvent);
			break;
		default:
			RUNTIME_ASSERT(false, "Received event has an unknown GenericEventType.\n");
		}
	}

	void DXRenderer::HandleStateEvent(Event::ICTMEvent* pEvent) noexcept
	{
		RUNTIME_ASSERT(pEvent != nullptr, "The provided event was nullptr.\n");
		RUNTIME_ASSERT(pEvent->GenericType() == Event::CTMGenericEventType::CTM_STATE_EVENT, "The provided event's GenericEventType wasn't CTM_STATE_EVENT.\n");

		switch (pEvent->ConcreteType())
		{
		case Event::CTMConcreteEventType::CTM_STATE_START_EVENT:
			OnStart(Event::CTMStartEvent::Cast(pEvent));
			break;
		case Event::CTMConcreteEventType::CTM_STATE_END_EVENT:
			OnEnd(Event::CTMEndEvent::Cast(pEvent));
			break;
		default:
			RUNTIME_ASSERT(false, "State event wasn't handled.\n");
		}
	}

	void DXRenderer::HandleMouseEvent(Event::ICTMEvent* pEvent) noexcept
	{
		RUNTIME_ASSERT(pEvent != nullptr, "The provided event was nullptr.\n");
		RUNTIME_ASSERT(pEvent->GenericType() == Event::CTMGenericEventType::CTM_MOUSE_EVENT, "The provided event's GenericEventType wasn't CTM_MOUSE_EVENT.\n");

		switch (pEvent->ConcreteType())
		{
		case Event::CTMConcreteEventType::CTM_MOUSE_MOVE_EVENT:
		{
			Event::CTMMouseMoveEvent* pMMoveEvent = Event::CTMMouseMoveEvent::Cast(pEvent);
			m_Window.Mouse().SetPos(pMMoveEvent->PosX(), pMMoveEvent->PosY());
			m_Window.SetTitle(std::wstring(L'(' + std::to_wstring(pMMoveEvent->PosX()) + L", " + std::to_wstring(pMMoveEvent->PosY()) + L')'));
			break;
		}
		default:
			RUNTIME_ASSERT(false, "Mouse event wasn't handled.\n");
		}
	}

	void DXRenderer::HandleFrameEvent(Event::ICTMEvent* pEvent) noexcept
	{
		RUNTIME_ASSERT(pEvent != nullptr, "The provided event was nullptr.\n");
		RUNTIME_ASSERT(pEvent->GenericType() == Event::CTMGenericEventType::CTM_FRAME_EVENT, "The provided event's GenericEventType wasn't CTM_FRAME_EVENT.\n");

		switch (pEvent->ConcreteType())
		{
		case Event::CTMConcreteEventType::CTM_FRAME_CLEAR_FRAME_EVENT:
			DEBUG_PRINT("Received an ClearFrameEvent.\n");
			m_Graphics.Clear();
			m_Graphics.Present();
			break;
		case Event::CTMConcreteEventType::CTM_FRAME_START_FRAME_EVENT:
			DEBUG_PRINT("Received an StartFrameEvent.\n");
			break;
		case Event::CTMConcreteEventType::CTM_FRAME_DRAW_FRAME_EVENT:
			DEBUG_PRINT("Received an DrawFrameEvent.\n");
			break;
		}
	}
	#pragma endregion
}
