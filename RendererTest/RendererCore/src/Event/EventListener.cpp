#include "CorePCH.hpp"
#include "Event/EventListener.hpp"

namespace Renderer::Event
{
	EventListener::EventListener(ListenType listenType, uint32_t id)
		: m_Type(listenType), m_ID(id)
	{

	}

	void EventListener::ActivateListener() noexcept
	{
		m_IsActive = true;
	}

	void EventListener::Notify(const Event* pEvent) noexcept
	{
		m_CurrentEvent = pEvent;
		m_IsNotified = true;
	}

	void EventListener::Clear() noexcept
	{
		m_IsNotified = false;
	}

	ListenType EventListener::GetListenTypeFromEvent(EventType eventType) noexcept
	{
		switch (eventType)
		{
		case EventType::INVALID:			return ListenType::INVALID;
		case EventType::RENDERER_START:		return ListenType::LISTEN_RENDERER_STATE;
		}
	}
}