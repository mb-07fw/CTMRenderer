#include "Core/CorePCH.hpp"
#include "Core/CoreMacros.hpp"
#include "Event/EventManager.hpp"

namespace Renderer::Event
{
	EventManager::EventManager()
		: m_ListenerPool(), m_EventPool(),
		  m_ListenerPoolMutex(), m_EventPoolMutex()
	{
		DEBUG_PRINT("[EventManager] Initialized EventManager.\n");
	}

	const std::weak_ptr<EventListener> EventManager::GetActiveListenerSafe(ListenType listenType) noexcept
	{
		std::lock_guard<std::mutex> lock(m_ListenerPoolMutex);

		// Return an empty weak_ptr if the listen type is invalid.
		if (listenType == ListenType::INVALID)
			return std::weak_ptr<EventListener>();
		
		const std::weak_ptr<EventListener> listener = m_ListenerPool.GetInactiveListener(listenType);

		m_ListenerPool.ActivateListener(listener);

		return listener;
	}
}