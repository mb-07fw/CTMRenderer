#pragma once

#include "CTMRenderer/ICTMRenderer.hpp"
#include "CTMRenderer/DirectX/DXRendererSettings.hpp"
#include "CTMRenderer/DirectX/Window/DXWindow.hpp"
#include "CTMRenderer/DirectX/Graphics/DXGraphics.hpp"

namespace CTMRenderer::CTMDirectX
{
	class DXRenderer : public ICTMRenderer
	{
	public:
		explicit DXRenderer(const unsigned int targetFPS);
		~DXRenderer() = default;
	public:
		virtual void Init() noexcept override;
		virtual void JoinForShutdown() noexcept override;
		virtual void ClearScreen() noexcept override;
	private:
		void OnStart(const Event::StartEvent* pEvent) noexcept;
		void OnEnd(const Event::EndEvent* pEvent) noexcept;
		void EventLoop() noexcept;
		void HandleEvent(Event::IEvent* pEvent) noexcept;
		void HandleStateEvent(Event::IEvent* pEvent) noexcept;
		void HandleMouseEvent(Event::IEvent* pEvent) noexcept;
		void HandleFrameEvent(Event::IEvent* pEvent) noexcept;
	private:
		DXRendererSettings m_Settings;
		Window::DXWindow m_Window;
		Graphics::DXGraphics m_Graphics;
	private:
		DXRenderer(const DXRenderer&) = delete;
		DXRenderer(DXRenderer&&) = delete;
		DXRenderer& operator=(const DXRenderer&) = delete;
		DXRenderer& operator=(DXRenderer&&) = delete;
	};
}