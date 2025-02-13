#pragma once

#include <Windows.h>

#include <d3d11.h>
#include <d3d11_1.h>
#include <d2d1_1.h>
#include <dwrite_1.h>

#include <wrl.h>
#include <DirectXMath.h>

#include <string_view>

#include "DirectX/Window/DXWindowGeometry.hpp"
#include "Control/Mouse.hpp"
#include "DirectX/Graphics/DXInfoQueue.hpp"
#include "DirectX/Graphics/DXBuffer.hpp"
#include "DirectX/Graphics/DXShader.hpp"
#include "DirectX/Graphics/DXShape.hpp"

namespace CTMRenderer::CTMDirectX::Window::Graphics
{
	struct Color
	{
		inline Color(float r = 0, float g = 0, float b = 0, float a = 0)
		{
			rgba[0] = r;
			rgba[1] = g;
			rgba[2] = b;
			rgba[3] = a;
		}

		float rgba[4];
	};

	struct Transform {
		DirectX::XMMATRIX transform;
	};

	struct Rendering2D {
		Microsoft::WRL::ComPtr<ID2D1Factory1> pFactory;
		Microsoft::WRL::ComPtr<ID2D1Device> pDevice;
		Microsoft::WRL::ComPtr<ID2D1DeviceContext> pDeviceContext;
		Microsoft::WRL::ComPtr<ID2D1RenderTarget> pRTV;
	};

	struct TextRender {
		Microsoft::WRL::ComPtr<IDWriteFactory> pDWriteFactory;
		Microsoft::WRL::ComPtr<IDWriteTextFormat> pTextFormat;
		Microsoft::WRL::ComPtr<ID2D1SolidColorBrush> pSCBrush;
		D2D1_RECT_F layoutRect;
		std::wstring_view text;
	};

	class Graphics
	{
	public:
		Graphics(const Geometry::WindowArea & windowArea, const Control::Mouse& mouseRef, const unsigned int targetFPS, const RECT& clientAreaRef) noexcept;
		Graphics(const Graphics&) = delete;
		Graphics(Graphics&&) = delete;
		Graphics& operator=(const Graphics&) = delete;
		Graphics& operator=(Graphics&&) = delete;
	public:
		void Init(const HWND windowHandle) noexcept;
		void Init2D() noexcept;
		void InitText() noexcept;
		void InitTestScene() noexcept;
		void StartFrame(double elapsedMillis) noexcept;
		void Draw() noexcept;
		void EndFrame() noexcept;
	private:
		void BindRTV() const noexcept;
	private:
		static constexpr unsigned char SYNC_INTERVAL = 1u;
	private:
		HWND m_WindowHandle;
		const Geometry::WindowArea& m_WindowAreaRef;
		const RECT& m_ClientAreaRef;
		Debug::InfoQueue m_InfoQueue;
		bool m_InitializedScene = false;
		Microsoft::WRL::ComPtr<ID3D11Device1> mP_Device;
		Microsoft::WRL::ComPtr<IDXGISwapChain> mP_SwapChain;
		Microsoft::WRL::ComPtr<ID3D11DeviceContext1> mP_DeviceContext;
		Microsoft::WRL::ComPtr<ID3D11RenderTargetView> mP_RTV;
		Rendering2D m_2DRender;
		TextRender m_TextRender;
		Color m_ClearColor;
		const unsigned int m_TargetFPS;
		const Control::Mouse& m_MouseRef;
	};
}