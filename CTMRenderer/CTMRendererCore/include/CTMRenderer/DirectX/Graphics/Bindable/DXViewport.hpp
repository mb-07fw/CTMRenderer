#pragma once

#include <d3d11.h>
#include <wrl/client.h>

namespace CTMRenderer::CTMDirectX::Graphics::Bindable
{
	class DXViewport
	{
	public:
		inline DXViewport(
			float topLeftX,
			float topLeftY,
			float width,
			float height,
			float minDepth,
			float maxDepth,
			Microsoft::WRL::ComPtr<ID3D11DeviceContext>& pContextRef) noexcept
			: mP_ContextRef(pContextRef)
		{
			m_Viewport.TopLeftX = topLeftX;
			m_Viewport.TopLeftY;
			m_Viewport.Width = width;
			m_Viewport.Height = height;
			m_Viewport.MinDepth = 0;
			m_Viewport.MaxDepth = 1;
		}
	public:
		inline void Bind() noexcept
		{
			mP_ContextRef->RSSetViewports(1, &m_Viewport);
		}
	private:
		Microsoft::WRL::ComPtr<ID3D11DeviceContext>& mP_ContextRef;
		D3D11_VIEWPORT m_Viewport = {};
	};
}