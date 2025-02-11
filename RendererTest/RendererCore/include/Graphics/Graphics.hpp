#pragma once

#include <Windows.h>
#include <d3d11.h>
#include <wrl.h>
#include <DirectXMath.h>

#include "Window/WindowGeometry.hpp"
#include "Window/Mouse.hpp"
#include "Graphics/DirectX/InfoQueue.hpp"
#include "Graphics/DirectX/Buffer.hpp"
#include "Graphics/DirectX/Shader.hpp"

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

	struct Transform {
		DirectX::XMMATRIX transform;
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
		Debug::InfoQueue m_InfoQueue;
		bool m_InitializedScene = false;
		Microsoft::WRL::ComPtr<ID3D11Device> mP_Device;
		Microsoft::WRL::ComPtr<IDXGISwapChain> mP_SwapChain;
		Microsoft::WRL::ComPtr<ID3D11DeviceContext> mP_DeviceContext;
		Microsoft::WRL::ComPtr<ID3D11RenderTargetView> mP_RTV;
		Microsoft::WRL::ComPtr<ID3D11Texture2D> mP_DSBuffer;
		Microsoft::WRL::ComPtr<ID3D11DepthStencilView> mP_DSView;
		CTMDirectX::ConstantBuffer<Transform, 1u, D3D11_CPU_ACCESS_WRITE> m_CBTransform;
		Microsoft::WRL::ComPtr<ID3D11Buffer> mP_CBRotation;
		Color m_ClearColor;
		const unsigned int m_TargetFPS;
		const Control::Mouse& m_MouseRef;
	};
}