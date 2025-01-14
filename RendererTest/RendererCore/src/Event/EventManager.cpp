#include "CorePCH.hpp"
#include "RendererMacros.hpp"
#include "Event/EventManager.hpp"

namespace Renderer::Event
{
	EventManager::EventManager()
	{
		DEBUG_PRINT("[EventManager] Initialized EventManager.\n");
	}

	void EventManager::PushEvent(EventType eventType)
	{
		m_ListenerPool.NotifyListeners(eventType);
	}

	const EventListener* EventManager::GetActiveListener(ListenType listenType)
	{
		if (listenType == ListenType::INVALID)
			return nullptr;

		EventListener* pListener = m_ListenerPool.GetInactiveListener(listenType);

		if (!m_ListenerPool.ActivateListener(pListener))
			DEBUG_PRINT_ERROR("[EventManager PUB.GetActiveListener] Failed to initialize listener : " + std::to_string(pListener->ID()) + '\n');

		return pListener;
	}
}