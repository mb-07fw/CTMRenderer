#include "Core/CorePCH.hpp"
#include "Core/CoreMacros.hpp"
#include "Event/ListenerPool.hpp"

namespace Renderer::Event
{
	void ListenerPool::NotifyListenersOfEvent(const Event* pEvent) noexcept
	{
		ListenType listenType = ListenTypeOfAbstractType(pEvent->AbsType());

		RUNTIME_ASSERT(listenType != ListenType::INVALID, "Correspondin listen type is invalid.\n");

		// Notify listeners that listen to all events with the EventType.
		NotifyListeners(m_ActiveListenerMap[ListenType::LISTEN_ALL], pEvent);
		
		// Notify listeners that listen to the specific EventType.
		NotifyListeners(m_ActiveListenerMap[listenType], pEvent);
	}

	const std::weak_ptr<EventListener> ListenerPool::GetInactiveListener(ListenType listenType)
	{
		// Get the vector of inactive listeners that's mapped to the listen type.
		std::vector<std::shared_ptr<EventListener>>& inactiveListeners = m_InactiveListenerMap[listenType];

		// Return an std::weak_ptr to a pooled listener if it's the vector isn't empty.
		if (!inactiveListeners.empty())
		{
			std::shared_ptr<EventListener>& listener = inactiveListeners.back();
			inactiveListeners.pop_back();
			return std::weak_ptr(listener);
		}

		// Construct a new listener with listener type.
		++m_TotalListeners;
		inactiveListeners.emplace_back(std::make_shared<EventListener>(listenType, m_TotalListeners));

		// Return an std::weak_ptr to the constructed listener.
		return std::weak_ptr(inactiveListeners.back());
	}

	bool ListenerPool::ActivateListener(const std::weak_ptr<EventListener>& listener)
	{
		// Lock the std::weak_ptr to get it's data safely.
		std::shared_ptr<EventListener> sharedListener = listener.lock();

		// Get listen type of the provided listener.
		ListenType listenType = sharedListener->Type();

		// Get references to listener vectors.
		std::vector<std::shared_ptr<EventListener>>& inactiveListeners = m_InactiveListenerMap[listenType];
		std::vector<std::shared_ptr<EventListener>>& activeListeners = m_ActiveListenerMap[listenType];

		// Get ID of the provided listener.
		int listenerID = sharedListener->ID();

		// Iterate through inactiveListeners to find the provided listener's ID.
		for (size_t i = 0; i < inactiveListeners.size(); ++i)
		{
			// Get reference to the listener.
			std::shared_ptr<EventListener>& inactiveListener = inactiveListeners.at(i);

			// Skip the listener if it doesn't match the provided listener's ID.
			if (inactiveListener->ID() != listenerID)
				continue;

			// Move the listener from inactiveListeners into activeListeners.
			activeListeners.emplace_back(std::move(inactiveListener));

			// Erase the moved listener from inactiveListeners.
			inactiveListeners.erase(inactiveListeners.begin() + i);

			// Activate the provided listener now that it's "owning" shared_ptr has been moved into activeListeners.
			sharedListener->ActivateListener();

			// Return true for successful.
			return true;
		}

		// Return false for unsuccessful.
		return false;
	}

	void ListenerPool::NotifyListeners(std::vector<std::shared_ptr<EventListener>>& activeListeners, const Event* pEvent) noexcept
	{
		// Notify all listeners in the listener vector with a pointer to the provided event.
		for (std::shared_ptr<EventListener>& listener : activeListeners)
			listener->Notify(pEvent);
	}

	constexpr ListenType ListenerPool::ListenTypeOfAbstractType(AbstractEventType absType) noexcept
	{
		switch (absType)
		{
		case AbstractEventType::RENDERER_STATE: return ListenType::LISTEN_RENDERER_STATE;
		default:								return ListenType::INVALID;
		}
	}
}