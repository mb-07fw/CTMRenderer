#include "Core/CorePCH.hpp"
#include "Core/CoreUtility.hpp"
#include "CTMRenderer/DirectX/Graphics/DXInfoQueue.hpp"

namespace CTMRenderer::CTMDirectX::Graphics::Debug
{
	void DXInfoQueue::Init(Microsoft::WRL::ComPtr<ID3D11Device1>& pDeviceRef) noexcept
	{
		// Get the info queue interface.
		HRESULT hResult = pDeviceRef->QueryInterface(__uuidof(ID3D11InfoQueue), &mP_InfoQueue);
		RUNTIME_ASSERT(hResult == S_OK, Utility::TranslateHResult(hResult));

		m_Initialized = true;
	}

	bool DXInfoQueue::IsQueueEmpty() const noexcept
	{
		AssertInitialized();

		return mP_InfoQueue->GetNumStoredMessages() == 0;
	}

	std::string DXInfoQueue::GetMessages() const noexcept
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

	void DXInfoQueue::AssertInitialized() const noexcept
	{
		RUNTIME_ASSERT(m_Initialized == true, "InfoQueue hasn't been initialized yet.");
	}
}