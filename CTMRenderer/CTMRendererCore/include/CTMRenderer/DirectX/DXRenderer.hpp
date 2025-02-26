#pragma once

#include "CTMRenderer/IRenderer.hpp"
#include "CTMRenderer/DirectX/DXRendererSettings.hpp"
#include "CTMRenderer/DirectX/Window/DXWindow.hpp"
#include "CTMRenderer/DirectX/Graphics/DXGraphics.hpp"

namespace CTMRenderer::CTMDirectX
{
	class DXRenderer : public IRenderer
	{
	public:
		explicit DXRenderer(const unsigned int targetFPS);
		~DXRenderer() = default;
	public:
		virtual void Start() noexcept override;
		virtual void JoinForShutdown() noexcept override;
		virtual void SubmitShape(const Shapes::IShape& shape) noexcept override;
	private:
		void OnStart(const Event::StartEvent* pEvent) noexcept;
		void OnEnd(const Event::EndEvent* pEvent) noexcept;
		void EventLoop() noexcept;
		void DoFrame(double elapsedMillis) noexcept;
		void HandleEvent(Event::IEvent* pEvent) noexcept;
		void HandleStateEvent(Event::IEvent* pEvent) noexcept;
		void HandleMouseEvent(Event::IEvent* pEvent) noexcept;
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