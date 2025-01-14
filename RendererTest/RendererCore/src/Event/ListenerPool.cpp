#include "CorePCH.hpp"
#include "RendererMacros.hpp"
#include "Event/ListenerPool.hpp"

namespace Renderer::Event
{
	void ListenerPool::NotifyListeners(EventType eventType) noexcept
	{
		const std::vector<std::unique_ptr<EventListener>>& listeners = m_ActiveListenerMap[ListenType::LISTEN_ALL];

		const std::unique_ptr<Event>& pEvent = GetPooledEvent(eventType);

		for (const std::unique_ptr<EventListener>& listener : listeners)
		{
			listener->Notify(pEvent.get());
			DEBUG_PRINT("[ListenerPool PUB.NotifyAll] Notified listener : " + std::to_string(listener->ID()) + '\n');
		}

		ListenType listenType = EventListener::GetListenTypeFromEvent(eventType);

		const std::vector<std::unique_ptr<EventListener>>& listenAll = m_ActiveListenerMap[listenType];

		for (const std::unique_ptr<EventListener>& listener : listenAll)
		{
			listener->Notify(pEvent.get());
			DEBUG_PRINT("[ListenerPool PUB.NotifyAll] Notified listener : " + std::to_string(listener->ID()) + '\n');
		}
	}

	EventListener* ListenerPool::GetInactiveListener(ListenType listenType)
	{
		if (m_InactiveListenerMap.size() == 0)
		{
			++m_TotalListeners;

			m_InactiveListenerMap[listenType].emplace_back(std::make_unique<EventListener>(listenType, m_TotalListeners));

			return m_InactiveListenerMap[listenType].back().get();
		}

		return nullptr;
	}

	bool ListenerPool::ActivateListener(EventListener*& pOutListener)
	{
		if (pOutListener == nullptr)
			return false;

		ListenType iterateType = pOutListener->Type();

		if (iterateType == ListenType::INVALID)
			return false;

		std::vector<std::unique_ptr<EventListener>>& inactiveListeners = m_InactiveListenerMap[iterateType];
		std::vector<std::unique_ptr<EventListener>>& activeListeners = m_ActiveListenerMap[iterateType];

		int pOutListenerID = pOutListener->ID();

		// Iterate through inactiveListeners to find the provided listener's.
		for (size_t i = 0; i < inactiveListeners.size(); ++i)
		{
			// Get reference to the listener.
			std::unique_ptr<EventListener>& listener = inactiveListeners.at(i);

			// Skip the listener if it doesn't match the provided listener's ID.
			if (listener->ID() != pOutListenerID)
				continue;

			// Move the listener from inactiveListeners into activeListeners.
			activeListeners.emplace_back(std::move(listener));

			// Erase the moved listener from inactiveListeners.
			inactiveListeners.erase(inactiveListeners.begin() + i);

			// Rewire the pointer to point to the moved listener.
			pOutListener = activeListeners.at(activeListeners.size() - 1).get();

			// Activate the listener.
			pOutListener->ActivateListener();

			// Return true for success.
			return true;
		}

		// Return false for unsuccessful.
		return false;
	}
	
	const std::unique_ptr<Event>& ListenerPool::GetPooledEvent(EventType eventType)
	{
		// TODO : Store events in an event pool.
		//		  This is returning a stack allocated object, idiot.
		return std::make_unique<Event>(eventType);
	}
}