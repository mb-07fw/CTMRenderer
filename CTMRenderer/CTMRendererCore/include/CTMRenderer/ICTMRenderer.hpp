#pragma once

#include <thread> // std::thread
#include <atomic> // std::atomic_bool
#include <mutex>  // std::mutex
#include <condition_variable> // std::condition_variable
#include <memory>

#include "CTMRenderer/Event/CTMEventSystem.hpp"
#include "CTMRenderer/CTMShapePool.hpp"
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
		void Start() noexcept;
		void Shutdown() noexcept;
		void JoinForShutdown() noexcept;

		void ClearScreen() noexcept;
		
		template <typename... Args>
		std::shared_ptr<Shapes::CTMRect> MakeRect(Args&&... args) noexcept;

		template <typename ShapeTy>
			requires std::is_base_of_v<Shapes::CTMShape, ShapeTy>
		void SubmitShape(const ShapeTy& shapeRef) noexcept;
		
		inline [[nodiscard]] bool IsRunning() const noexcept { return m_ShouldRun.load(std::memory_order_acquire); }
	private:
		virtual std::thread StartEventThread() noexcept = 0;

		/*template <typename ShapeTy, typename... Args>
		[[nodiscard]] ShapeTy& MakeShape(Args&&... args) noexcept;*/
	protected:
		Event::CTMEventSystem m_EventSystem;
		CTMShapePool m_ShapePool;
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

	template <typename... Args>
	std::shared_ptr<Shapes::CTMRect> ICTMRenderer::MakeRect(Args&&... args) noexcept
	{
		return m_ShapePool.GetPooledRect(std::forward<Args>(args)...);
	}

	template <typename ShapeTy>
		requires std::is_base_of_v<Shapes::CTMShape, ShapeTy>
	void ICTMRenderer::SubmitShape(const ShapeTy& shapeRef) noexcept
	{
		m_DrawQueue.QueueShape(shapeRef);
	}


	/*template <typename ShapeTy, typename... Args>
	[[nodiscard]] ShapeTy& ICTMRenderer::MakeShape(Args&&... args) noexcept
	{

	}*/
}