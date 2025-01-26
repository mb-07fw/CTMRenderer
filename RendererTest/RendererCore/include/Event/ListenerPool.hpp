#pragma once

#include <unordered_map> // std::unordered_map
#include <vector>		 // std::vector
#include <memory>		 // std::shared_ptr
#include <cstdint>		 // uint32t

#include "EventListener.hpp"
#include "EventPool.hpp"

namespace Renderer::Event
{
	class ListenerPool
	{
	public:
		ListenerPool() = default;
		ListenerPool(const ListenerPool&) = delete;
		ListenerPool(ListenerPool&&) = delete;
		ListenerPool& operator=(const ListenerPool&) = delete;
		ListenerPool& operator=(ListenerPool&&) = delete;
	public:
		void NotifyListenersOfEvent(const Event* pEvent) noexcept;

		// Returns an inactive listener that's compatible with the provided ListenType.
		[[nodiscard]] const std::weak_ptr<EventListener> GetInactiveListener(ListenType listenType);

		/* Activates the provided listener to listen for events.
		 * Returns true if the listener was activated. Returns false if not, or if the listener was already active. */
		bool ActivateListener(const std::weak_ptr<EventListener>& listener);
	private:
		void NotifyListeners(std::vector<std::shared_ptr<EventListener>>& activeListeners, const Event* pEvent) noexcept;
	private:
		static constexpr ListenType ListenTypeOfAbstractType(AbstractEventType absType) noexcept;
	private:
		std::unordered_map<ListenType, std::vector<std::shared_ptr<EventListener>>> m_ActiveListenerMap;
		std::unordered_map<ListenType, std::vector<std::shared_ptr<EventListener>>> m_InactiveListenerMap;
		EventPool m_EventPool;
		uint32_t m_TotalListeners = 0;
	};
}