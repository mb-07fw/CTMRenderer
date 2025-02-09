#pragma once

#include <Windows.h>
#include <d3d11.h>
#include <wrl.h>
#include <DirectXMath.h>

#include "Geometry.hpp"
#include "InfoQueue.hpp"
#include "Mouse.hpp"

namespace CTMRenderer::Window::Graphics
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

	class Graphics
	{
	public:
		Graphics(const Geometry::WindowArea & windowArea, const Control::Mouse& mouseRef, const unsigned int targetFPS) noexcept;
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
		static constexpr unsigned char SYNC_INTERVAL = 1u;
	private:
		HWND m_WindowHandle;
		const Geometry::WindowArea& m_WindowAreaRef;
		Debug::InfoQueue m_InfoQueue;
		bool m_InitializedScene;
		Microsoft::WRL::ComPtr<ID3D11Device> mP_Device;
		Microsoft::WRL::ComPtr<IDXGISwapChain> mP_SwapChain;
		Microsoft::WRL::ComPtr<ID3D11DeviceContext> mP_DeviceContext;
		Microsoft::WRL::ComPtr<ID3D11RenderTargetView> mP_RTV;
		Microsoft::WRL::ComPtr<ID3D11Buffer> mP_ConstantBuffer;
		Color m_ClearColor;
		const unsigned int m_TargetFPS;
		const Control::Mouse& m_MouseRef;
	private:
		struct Transform {
			DirectX::XMFLOAT2 translation;
			float padding[2];
		};
	};
}