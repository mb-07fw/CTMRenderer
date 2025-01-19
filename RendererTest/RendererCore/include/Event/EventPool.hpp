#pragma once

#include <unordered_map>
#include <vector>
#include <memory>

#include "Event.hpp"

namespace Renderer::Event
{
	class EventPool
	{
	public:
		EventPool() = default;
		EventPool(const EventPool&) = delete;
		EventPool(EventPool&&) = delete;
		EventPool& operator=(const EventPool&) = delete;
		EventPool& operator=(EventPool&&) = delete;
	public:
		/* Gets a pooled event that corresponds to the provided EventType.
		 * TODO: Return a pointer in the future for polymorphic Event types. */
		const Event& GetPooledEvent(EventType eventType);
	private:
		std::unordered_map<EventType, std::vector<std::unique_ptr<Event>>> m_EventPool;
	};
}