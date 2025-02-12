#include "Core/CorePCH.hpp"
#include "Core/CoreUtility.hpp"
#include "DirectX/Graphics/DXInfoQueue.hpp"

namespace CTMRenderer::CTMDirectX::Window::Graphics::Debug
{
	InfoQueue::InfoQueue() noexcept
		: mP_InfoQueue(nullptr), m_Initialized(false)
	{

	}

	void InfoQueue::Init(Microsoft::WRL::ComPtr<ID3D11Device>& pDeviceRef) noexcept
	{
		// Get the info queue interface.
		HRESULT hResult = pDeviceRef->QueryInterface(__uuidof(ID3D11InfoQueue), &mP_InfoQueue);
		RUNTIME_ASSERT(hResult == S_OK, Utility::TranslateHResult(hResult));

		m_Initialized = true;
	}

	bool InfoQueue::IsQueueEmpty() const noexcept
	{
		AssertInitialized();

		return mP_InfoQueue->GetNumStoredMessages() == 0;
	}

	std::string InfoQueue::GetMessagesAsStr() const noexcept
	{
		AssertInitialized();

		std::string messageStr;
		size_t messageLength = 0;

		HRESULT hResult = S_OK;
		for (size_t i = 0; i < mP_InfoQueue->GetNumStoredMessages(); i++)
		{
			// Get size of message.
			mP_InfoQueue->GetMessageW(i, nullptr, &messageLength);

			if (messageLength <= 0)
				continue;

			// Specify deleter with message.
			std::unique_ptr<D3D11_MESSAGE, decltype(&free)> pMessage(reinterpret_cast<D3D11_MESSAGE*>(malloc(messageLength)), free);

			RUNTIME_ASSERT(pMessage != nullptr, "Received message is nullptr.\n");

			// Get message.
			hResult = mP_InfoQueue->GetMessageW(i, pMessage.get(), &messageLength);
			RUNTIME_ASSERT(hResult == S_OK, Utility::TranslateHResult(hResult));

			messageStr.append(pMessage->pDescription);
		}

		return messageStr;
	}

	void InfoQueue::AssertInitialized() const noexcept
	{
		RUNTIME_ASSERT(m_Initialized == true, "InfoQueue hasn't been initialized yet.");
	}
}