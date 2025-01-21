#pragma once

#include <Windows.h>
#include <d3d11.h>
#include <wrl.h>

#include "Geometry.hpp"
#include "Color.hpp"
#include "InfoQueue.hpp"

namespace Renderer::Window::Graphics
{
	class Graphics
	{
	public:
		Graphics(const Geometry::WindowArea& windowArea) noexcept;
		Graphics(const Graphics&) = delete;
		Graphics(Graphics&&) = delete;
		Graphics& operator=(const Graphics&) = delete;
		Graphics& operator=(Graphics&&) = delete;
	public:
		void Init(const HWND windowHandle) noexcept;
		void InitTestScene() noexcept;
		void StartFrame() noexcept;
		void Draw() noexcept;
		void EndFrame() noexcept;
	private:
		void BindRTV() const noexcept;
	private:
		static constexpr unsigned int SYNC_INTERVAL = 1u;
	private:
		HWND m_WindowHandle;
		const Geometry::WindowArea& m_WindowArea;
		Debug::InfoQueue m_InfoQueue;
		bool m_InitializedScene;
		Microsoft::WRL::ComPtr<ID3D11Device> mP_Device;
		Microsoft::WRL::ComPtr<IDXGISwapChain> mP_SwapChain;
		Microsoft::WRL::ComPtr<ID3D11DeviceContext> mP_DeviceContext;
		Microsoft::WRL::ComPtr<ID3D11RenderTargetView> mP_RTV;
		Color m_ClearColor;
	};
}