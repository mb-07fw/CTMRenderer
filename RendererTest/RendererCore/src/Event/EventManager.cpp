#include "Core/CorePCH.hpp"
#include "Core/CoreMacros.hpp"
#include "Event/EventManager.hpp"

namespace Renderer::Event
{
	EventManager::EventManager()
	{
		DEBUG_PRINT("[EventManager] Initialized EventManager.\n");
	}

	void EventManager::BroadcastEvent(EventType eventType) noexcept
	{
		m_ListenerPool.NotifyListenersOfEvent(eventType);
	}

	const std::weak_ptr<EventListener> EventManager::GetActiveListener(ListenType listenType) noexcept
	{
		// Return an empty weak_ptr if the listen type is invalid.
		if (listenType == ListenType::INVALID)
			return std::weak_ptr<EventListener>();
		
		const std::weak_ptr<EventListener> listener = m_ListenerPool.GetInactiveListener(listenType);

		m_ListenerPool.ActivateListener(listener);

		return listener;
	}
}