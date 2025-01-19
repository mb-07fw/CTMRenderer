#pragma once

#include <mutex>
#include <memory>

#include "EventManager.hpp"

namespace Renderer::Event
{
	// Protects EventManager from asynchronous calls by locking a mutex during calls.
	// Requires that all callers have the same reference to the interface.
	class EventManagerInterface
	{
	public:
		EventManagerInterface(EventManager& eventManager) noexcept;
	public:
		void BroadcastEventSafe(EventType eventType) noexcept;

		[[nodiscard]] const std::weak_ptr<EventListener> GetActiveListenerSafe(ListenType listenType) noexcept;

		// Not recommended, due to no protection against multi-threaded interactions.
		inline EventManager& Get() { return m_EventManager; }
	private:
		EventManager& m_EventManager;
		std::mutex m_Mutex;
	};
}