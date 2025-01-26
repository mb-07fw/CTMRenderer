#pragma once

#include <unordered_map>
#include <vector>
#include <memory>

#include "Event.hpp"
#include "Core/CoreMacros.hpp"

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
		// Returns a pooled event via either construction, or updating of a previous event.
		template <typename T, ConcreteEventType ConcreteType, typename... Args>
		requires std::is_base_of_v<Event, T> && IsMatchingConcreteEventType<T, ConcreteType>::value
		T* GetPooledEvent(Args&&... args) noexcept;
	private:
		template <ConcreteEventType ConcreteType, typename... Args>
		void UpdateEvent(Event* pEvent, Args&&... args) const noexcept;
	private:
		std::unordered_map<ConcreteEventType, std::vector<std::unique_ptr<Event>>> m_PooledEvents;
	};

	template <typename T, ConcreteEventType ConcreteType, typename... Args>
	requires std::is_base_of_v<Event, T> && IsMatchingConcreteEventType<T, ConcreteType>::value
	T* EventPool::GetPooledEvent(Args&&... args) noexcept
	{
		static_assert(IsMatchingConcreteEventType<T, ConcreteType>::value, "T does not match the specified ConcreteEventType");

		std::vector<std::unique_ptr<Event>>& pooledEvents = m_PooledEvents[ConcreteType];

		// No previous events exist.
		if (pooledEvents.empty())
			pooledEvents.emplace_back(std::make_unique<T>(std::forward<Args>(args)...));

		// Update the previous event.
		else
		{
			Event* pEvent = pooledEvents.back().get();

			// Since the vectors are mapped by ConcreteEventType's, this is pretty much impossible to be unsafe unless I do 
			// something incomprehensibly dumb like putting an event in the wrong container.
			// Either way, better safe than seg-faulting.
			RUNTIME_ASSERT(pEvent->ConcreteType() == ConcreteType, "Concrete types mismatch.\n");
			
			UpdateEvent<ConcreteType>(pEvent, std::forward<Args>(args)...);

			return static_cast<T*>(pEvent);
		}
		
		return static_cast<T*>(pooledEvents.back().get());
	}

	template <ConcreteEventType ConcreteType, typename... Args>
	void EventPool::UpdateEvent(Event* pEvent, Args&&... args) const noexcept
	{
		switch (ConcreteType)
		{
		case ConcreteEventType::RENDERER_START:
			static_cast<RendererStartEvent*>(pEvent)->Update((unsigned int)std::forward<Args>(args)...);
			break;
		case ConcreteEventType::RENDERER_END:
			static_cast<RendererEndEvent*>(pEvent)->Update((float)std::forward<Args>(args)...);
			break;
		}
	}
}