#pragma once

#include "CorePCH.hpp" // std::
#include "EventListener.hpp"

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
		void NotifyListeners(EventType eventType) noexcept;
		[[nodiscard]] EventListener* GetInactiveListener(ListenType listenType);

		/* Rewires the provided pointer to point to the moved EventListener
		*  from a map of inactive listeners to active listeners.
		*
		*  Returns true if the listener is active. Returns false if not.
		*/
		bool ActivateListener(EventListener*& pOutListener);
	private:
		[[nodiscard]] const std::unique_ptr<Event>& GetPooledEvent(EventType eventType);
	private:
		std::unordered_map<ListenType, std::vector<std::unique_ptr<EventListener>>> m_ActiveListenerMap;
		std::unordered_map<ListenType, std::vector<std::unique_ptr<EventListener>>> m_InactiveListenerMap;
		uint32_t m_TotalListeners = 0;
	};
}