#pragma once

#include "Event/Event.hpp"
#include "ListenerPool.hpp"

namespace Renderer::Event
{
	class EventManager
	{
	public:
		EventManager();
		EventManager(const EventManager&) = delete;
		EventManager(EventManager&&) = delete;
		EventManager& operator=(const EventManager&) = delete;
		EventManager& operator=(EventManager&&) = delete;
	public:
		void PushEvent(EventType eventType);
		
		/* Returns an pointer to a listener.
		*  Return value may be nullptr if ListenType == INVALID.
		*  The returned pointer must not be deleted, as it is already managed internally via std::unique_ptr.
		*  Call DeactivateListener to deactivate the listener.
		*/
		[[nodiscard]] const EventListener* GetActiveListener(ListenType listenType);
	private:
		ListenerPool m_ListenerPool;
	};
}