#include "Core/CorePCH.hpp"
#include "Event/EventDispatcher.hpp"

namespace CTMRenderer::Event
{
	EventDispatcher::EventDispatcher() noexcept
		: m_EventPool(), m_GenericListeners(), m_ConcreteListeners() {}

	void EventDispatcher::Subscribe(IGenericListener* pGenericListener) noexcept
	{
		RUNTIME_ASSERT(pGenericListener != nullptr, "Recieved generic listener is nullptr.");

		m_GenericListeners[pGenericListener->ListenType()].emplace_back(pGenericListener);
	}
	
	void EventDispatcher::Subscribe(IConcreteListener* pConcreteListener) noexcept
	{
		RUNTIME_ASSERT(pConcreteListener != nullptr, "Recieved concrete listener is nullptr.");

		m_ConcreteListeners[pConcreteListener->ListenType()].emplace_back(pConcreteListener);
	}

	void EventDispatcher::Unsubscribe(IGenericListener* pGenericListener) noexcept
	{
		RUNTIME_ASSERT(pGenericListener != nullptr, "Recieved concrete listener is nullptr.");

		std::vector<IGenericListener*>& genericListeners = m_GenericListeners[pGenericListener->ListenType()];

		// If the pointer of the provided listener matches the stored listener, remove it from storage.
		for (size_t i = 0; i < genericListeners.size(); ++i)
			if (pGenericListener == genericListeners[i])
				genericListeners.erase(genericListeners.begin() + i);
	}

	void EventDispatcher::Unsubscribe(IConcreteListener* pConcreteListener) noexcept
	{
		RUNTIME_ASSERT(pConcreteListener != nullptr, "Recieved concrete listener is nullptr.");

		std::vector<IConcreteListener*>& concreteListeners = m_ConcreteListeners[pConcreteListener->ListenType()];

		// If the pointer of the provided listener matches the stored listener, remove it from storage.
		for (size_t i = 0; i < concreteListeners.size(); ++i)
			if (pConcreteListener == concreteListeners[i])
				concreteListeners.erase(concreteListeners.begin() + i);
	}
}