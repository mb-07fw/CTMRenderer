#pragma once

namespace Renderer::Window::Graphics::Debug
{
	class InfoQueue
	{
	public:
		InfoQueue() noexcept;
	public:
		void Init(Microsoft::WRL::ComPtr<ID3D11Device>& pDeviceRef) noexcept;

		[[nodiscard]] bool IsQueueEmpty() const noexcept;
		[[nodiscard]] std::string GetMessagesAsStr() const noexcept;
	private:
		void AssertInitialized() const noexcept;
	private:
		Microsoft::WRL::ComPtr<ID3D11InfoQueue> mP_InfoQueue;
		bool m_Initialized;
	};
}