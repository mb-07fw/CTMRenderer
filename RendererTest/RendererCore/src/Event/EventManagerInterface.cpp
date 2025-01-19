#include "Core/CorePCH.hpp"
#include "Core/CoreMacros.hpp"
#include "Event/EventManagerInterface.hpp"

namespace Renderer::Event
{
	EventManagerInterface::EventManagerInterface(EventManager& eventManager) noexcept
		: m_EventManager(eventManager), m_Mutex()
	{

	}

	void EventManagerInterface::BroadcastEventSafe(EventType eventType) noexcept
	{
		std::lock_guard<std::mutex> lock(m_Mutex);
		m_EventManager.BroadcastEvent(eventType);
	}

	const std::weak_ptr<EventListener> EventManagerInterface::GetActiveListenerSafe(ListenType listenType) noexcept
	{
		std::lock_guard<std::mutex> lock(m_Mutex);
		return m_EventManager.GetActiveListener(listenType);
	}
}