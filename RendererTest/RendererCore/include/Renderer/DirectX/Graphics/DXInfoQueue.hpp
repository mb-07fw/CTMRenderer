#pragma once

#include <d3d11_1.h>

namespace CTMRenderer::CTMDirectX::Graphics::Debug
{
	class DXInfoQueue
	{
	public:
		DXInfoQueue() = default;
		~DXInfoQueue() = default;
	public:
		void Init(Microsoft::WRL::ComPtr<ID3D11Device1>& pDeviceRef) noexcept;
		[[nodiscard]] bool IsQueueEmpty() const noexcept;
		[[nodiscard]] std::string GetMessages() const noexcept;
	private:
		void AssertInitialized() const noexcept;
	private:
		Microsoft::WRL::ComPtr<ID3D11InfoQueue> mP_InfoQueue;
		bool m_Initialized = false;;
	};
}