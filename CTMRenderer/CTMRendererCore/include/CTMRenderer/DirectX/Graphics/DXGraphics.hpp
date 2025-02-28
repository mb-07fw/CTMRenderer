#pragma once

#include <Windows.h>

#include <d3d11.h>
#include <d3d11_1.h>
#include <d2d1_1.h>
#include <dwrite_1.h>
#include <wrl.h>
#include <DirectXMath.h>

#include <string_view>

#include "CTMRenderer/DirectX/Control/Mouse.hpp"
#include "CTMRenderer/DirectX/DXRendererSettings.hpp"
#include "CTMRenderer/DirectX/Window/DXWindowGeometry.hpp"
#include "CTMRenderer/DirectX/Graphics/DXInfoQueue.hpp"
#include "CTMRenderer/DirectX/Graphics/DXShaderLibrary.hpp"
#include "CTMRenderer/CTMColor.hpp"

namespace CTMRenderer::CTMDirectX::Graphics
{
	struct Transform {
		DirectX::XMMATRIX transform;
	};

	// Temporary data containers for initial text rendering with DirectWrite implementation.
	struct Rendering2D {
		Microsoft::WRL::ComPtr<ID2D1Factory1> pFactory;
		Microsoft::WRL::ComPtr<ID2D1Device> pDevice;
		Microsoft::WRL::ComPtr<ID2D1DeviceContext> pDeviceContext;
		Microsoft::WRL::ComPtr<ID2D1RenderTarget> pRTV;
	};

	// Temporary data containers for initial text rendering with DirectWrite implementation.
	struct TextRender {
		Microsoft::WRL::ComPtr<IDWriteFactory> pDWriteFactory;
		Microsoft::WRL::ComPtr<IDWriteTextFormat> pTextFormat;
		Microsoft::WRL::ComPtr<ID2D1SolidColorBrush> pSCBrush;
		D2D1_RECT_F layoutRect = {};
		std::wstring_view text;
	};

	class DXGraphics
	{
	public:
		DXGraphics(const DXRendererSettings& settingsRef, const Window::Geometry::WindowArea& windowAreaRef, const Control::Mouse& mouseRef) noexcept;
		~DXGraphics() = default;
	public:
		void Init(const HWND windowHandle) noexcept;
		void SetClearColor(Shapes::CTMNormColor normColor) noexcept;
		void Clear() noexcept;
		void Present() noexcept;
		void StartFrame(double elapsedMillis) noexcept;
		void Draw() noexcept;
		void EndFrame() noexcept;
	private:
		void Init2D() noexcept;
		void InitText() noexcept;
		void InitTestScene() noexcept;
		void BindRTV() const noexcept;
	private:
		static constexpr unsigned char SYNC_INTERVAL = 1u;
	private:
		const DXRendererSettings& m_SettingsRef;
		const Window::Geometry::WindowArea& m_WindowAreaRef;
		const Control::Mouse& m_MouseRef;
		Debug::DXInfoQueue m_InfoQueue;
		DXShaderLibrary m_ShaderLibrary;
		Microsoft::WRL::ComPtr<ID3D11Device1> mP_Device;
		Microsoft::WRL::ComPtr<IDXGISwapChain> mP_SwapChain;
		Microsoft::WRL::ComPtr<ID3D11DeviceContext1> mP_DeviceContext;
		Microsoft::WRL::ComPtr<ID3D11RenderTargetView> mP_RTV;
		bool m_InitializedScene = false;
		HWND m_WindowHandle = nullptr;
		Rendering2D m_2DRender;
		TextRender m_TextRender;
		Shapes::CTMNormColor m_ClearColor;
	private:
		DXGraphics(const DXGraphics&) = delete;
		DXGraphics(DXGraphics&&) = delete;
		DXGraphics& operator=(const DXGraphics&) = delete;
		DXGraphics& operator=(DXGraphics&&) = delete;
	};
}