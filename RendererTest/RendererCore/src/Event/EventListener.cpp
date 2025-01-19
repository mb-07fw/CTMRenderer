#include "Core/CorePCH.hpp"
#include "Core/CoreMacros.hpp"
#include "Event/EventListener.hpp"

namespace Renderer::Event
{
	EventListener::EventListener(ListenType listenType, int id)
		: m_Type(listenType), m_ID(id), m_IsActive(false), m_IsNotified(false), m_EventQueue()
	{

	}

	void EventListener::ActivateListener() noexcept
	{
		m_IsActive = true;
	}

	void EventListener::Notify(const Event* event) noexcept
	{
		m_IsNotified = true;

		m_EventQueue.emplace_back(event);
	}

	void EventListener::ClearNotification() noexcept
	{
		m_IsNotified = false;
	}

	void EventListener::ClearQueue() noexcept
	{
		m_EventQueue.clear();
	}

	std::string EventListener::IDToStr() const noexcept
	{
		return std::to_string(m_ID);
	}

	const Event* EventListener::PopLatest() noexcept
	{
		RUNTIME_ASSERT(m_EventQueue.empty() == false, "No events to return.\n");

		const Event* latestEvent = m_EventQueue.back();

		m_EventQueue.pop_back();

		return latestEvent;
	}

	const Event* EventListener::PopOldest() noexcept
	{
		RUNTIME_ASSERT(m_EventQueue.empty() == false, "No events to return.\n");

		const Event* latestEvent = m_EventQueue.front();

		m_EventQueue.pop_front();

		return latestEvent;
	}

	bool EventListener::IsEventQueueEmpty() const noexcept
	{
		return m_EventQueue.empty();
	}
}