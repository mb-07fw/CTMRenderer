#pragma once

#include <thread> // std::thread
#include <atomic> // std::atomic_bool
#include <mutex>  // std::mutex
#include <condition_variable> // std::condition_variable

#include "CTMRenderer/Event/CTMEventSystem.hpp"
#include "CTMRenderer/CTMDrawQueue.hpp"
#include "CTMRenderer/CTMTimer.hpp"
#include "CTMRenderer/CTMShape.hpp"

namespace CTMRenderer
{
	enum class CTMRendererType
	{
		CTM_DIRECTX
	};

	class ICTMRenderer
	{
	public:
		ICTMRenderer() = default;
		virtual ~ICTMRenderer() = default;
	public:
		virtual void Init() noexcept = 0;
		virtual void JoinForShutdown() noexcept = 0;
		void ClearScreen() noexcept;
		void SubmitShape(const Shapes::CTMShape& shapeRef) noexcept;

		/*template <typename ShapeTy, typename... Args>
		[[nodiscard]] ShapeTy& MakeShape(Args&&... args) noexcept;*/
	protected:
		Event::CTMEventSystem m_EventSystem;
		CTMDrawQueue m_DrawQueue;
		Utils::CTMTimer m_Timer;
		std::thread m_EventThread;
		std::mutex m_RendererMutex;
		std::condition_variable m_RendererCV;
		std::atomic_bool m_ShouldRun = false;
		std::atomic_bool m_EventLoopStarted = false;
		std::atomic_bool m_RendererStarted = false;
	private:
		ICTMRenderer(const ICTMRenderer&) = delete;
		ICTMRenderer(ICTMRenderer&&) = delete;
		ICTMRenderer& operator=(const ICTMRenderer&) = delete;
		ICTMRenderer& operator=(ICTMRenderer&&) = delete;
	};

	/*template <typename ShapeTy, typename... Args>
	[[nodiscard]] ShapeTy& ICTMRenderer::MakeShape(Args&&... args) noexcept
	{

	}*/
}