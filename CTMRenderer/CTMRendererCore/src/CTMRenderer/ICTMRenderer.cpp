#include "Core/CorePCH.hpp"
#include "CTMRenderer/ICTMRenderer.hpp"

namespace CTMRenderer
{
	void ICTMRenderer::Start() noexcept
	{
		RUNTIME_ASSERT(!m_ShouldRun && !m_RendererStarted, "The renderer has already started, or is already running.\n");

		m_ShouldRun.store(true, std::memory_order_release);
		m_EventThread = StartEventThread();

		// Wait for the event loop to start.
		std::unique_lock<std::mutex> lock(m_RendererMutex);
		m_RendererCV.wait(lock, [this] { return m_EventLoopStarted.load(std::memory_order_acquire); });

		m_EventSystem.Dispatcher().QueueEvent<Event::CTMStartEvent>(1738u); // ayy

		//DEBUG_PRINT("Initialized renderer.\n");
	}

	void ICTMRenderer::Shutdown() noexcept
	{
		m_EventSystem.Dispatcher().QueueEvent<Event::CTMEndEvent>(1107u);
	}

	void ICTMRenderer::JoinForShutdown() noexcept
	{
		m_EventThread.join();
	}

	void ICTMRenderer::ClearScreen() noexcept
	{
		m_EventSystem.Dispatcher().QueueEvent<Event::CTMClearFrameEvent>();
	}
}