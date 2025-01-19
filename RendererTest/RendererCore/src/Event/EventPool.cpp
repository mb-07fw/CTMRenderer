#include "Core/CorePCH.hpp"
#include "Event/EventPool.hpp"

namespace Renderer::Event
{
	const Event& EventPool::GetPooledEvent(EventType eventType)
	{
		std::vector<std::unique_ptr<Event>>& pooledEvents = m_EventPool[eventType];

		if (pooledEvents.empty())
			pooledEvents.emplace_back(std::make_unique<Event>(eventType));

		// TODO : Implement updating and recycling of a pooled event.

		return *pooledEvents.back().get();
	}
}