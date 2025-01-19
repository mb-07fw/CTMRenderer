#include "Core/CorePCH.hpp"
#include "Core/CoreMacros.hpp"
#include "Event/ListenerPool.hpp"

namespace Renderer::Event
{
	void ListenerPool::NotifyListenersOfEvent(EventType eventType) noexcept
	{
		ListenType listenType = GetListenTypeOfEventType(eventType);

		RUNTIME_ASSERT(listenType != ListenType::INVALID, "Listen type is invalid.\n");

		// Notify listeners that listen to all events with the EventType.
		NotifyListeners(m_ActiveListenerMap[ListenType::LISTEN_ALL], eventType);
		
		// Notify listeners that listen to the specific EventType.
		NotifyListeners(m_ActiveListenerMap[listenType], eventType);
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
		ListenType iterateType = sharedListener->Type();

		// Get references to listener vectors.
		std::vector<std::shared_ptr<EventListener>>& inactiveListeners = m_InactiveListenerMap[iterateType];
		std::vector<std::shared_ptr<EventListener>>& activeListeners = m_ActiveListenerMap[iterateType];

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

			// Activate the listener.
			sharedListener->ActivateListener();

			// Return true for successful.
			return true;
		}

		// Return false for unsuccessful.
		return false;
	}

	void ListenerPool::NotifyListeners(std::vector<std::shared_ptr<EventListener>>& activeListeners, EventType eventType) noexcept
	{
		// Get a pooled event with the event type.
		const Event& event = m_EventPool.GetPooledEvent(eventType);

		// Notify all listeners in the listener vector with a pointer to the provided event.
		for (std::shared_ptr<EventListener>& listener : activeListeners)
			listener->Notify(&event);
	}

	constexpr ListenType ListenerPool::GetListenTypeOfEventType(EventType eventType) noexcept
	{
		switch (eventType)
		{
		case EventType::RENDERER_START:
		case EventType::RENDERER_END:   return ListenType::LISTEN_RENDERER_STATE;
		case EventType::INVALID:
		default:						return ListenType::INVALID;
		}
	}
}