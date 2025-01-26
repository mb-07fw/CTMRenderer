#pragma once

#include <mutex>
#include <type_traits>

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
		/* Requires: T is derived from Event.
		 *			 The number of arguments provided matches the amount necessary for the Event specification.
		 */
		template <typename T, ConcreteEventType ConcreteType, typename... Args>
		requires std::is_base_of_v<Event, T> && (EventArgsCount<T, ConcreteType>::value == sizeof...(Args))
		void BroadcastEventSafe(Args&&... args) noexcept;

		// Returns a const std::weak_ptr to an an EventListener that listens for the provided ListenType.
		[[nodiscard]] const std::weak_ptr<EventListener> GetActiveListenerSafe(ListenType listenType) noexcept;
	private:
		ListenerPool m_ListenerPool;
		EventPool m_EventPool;
		std::mutex m_ListenerPoolMutex;
		std::mutex m_EventPoolMutex;
	};

	//#include "EventManager.tpp"

	template <typename T, ConcreteEventType ConcreteType, typename... Args>
	requires std::is_base_of_v<Event, T> && (EventArgsCount<T, ConcreteType>::value == sizeof...(Args))
	void EventManager::BroadcastEventSafe(Args&&... args) noexcept
	{
		T* pEvent = nullptr;
		{
			std::lock_guard<std::mutex> lock(m_EventPoolMutex);
			pEvent = m_EventPool.GetPooledEvent<T, ConcreteType>(std::forward<Args>(args)...);
		}

		RUNTIME_ASSERT(pEvent != nullptr, "Event is nullptr.\n");

		{
			std::lock_guard<std::mutex> lock(m_ListenerPoolMutex);
			m_ListenerPool.NotifyListenersOfEvent(pEvent);
		}
	}
}