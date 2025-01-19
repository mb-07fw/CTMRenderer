#pragma once

#include <functional>

#include "Event/Event.hpp"
#include "ListenerPool.hpp"
#include "EventPool.hpp"

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
		void BroadcastEvent(EventType eventType) noexcept;
		
		// Returns a const std::weak_ptr to an an EventListener that listens for the provided ListenType.
		[[nodiscard]] const std::weak_ptr<EventListener> GetActiveListener(ListenType listenType) noexcept;
	private:
		ListenerPool m_ListenerPool;
	};
}