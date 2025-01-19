#pragma once

#include <deque>

#include "Event.hpp"

namespace Renderer::Event
{
	enum class ListenType
	{
		INVALID = -1,			// A marker for any Listener's in invalid states.
		LISTEN_ALL,				// Listen to all events across Renderer. (Graphics events, window events, etc)
		LISTEN_RENDERER_STATE	// Listen to all events involving Renderer state. (Start, shutdown, etc)
	};

	class EventListener
	{
	public:
		EventListener(ListenType listenType = ListenType::INVALID, int id = -1);
		EventListener(const EventListener&) = delete;
		EventListener(EventListener&&) = delete;
		EventListener& operator=(const EventListener&) = delete;
		EventListener& operator=(EventListener&&) = delete;
	public:
		// Sets the activated status to true.
		void ActivateListener() noexcept;

		// Sets the current event to the event provided, and sets the notified status to true.
		void Notify(const Event* event) noexcept;

		// Resets the notified status of the listener.
		void ClearNotification() noexcept;

		void ClearQueue() noexcept;

		// Uses std::to_string() to convert the ID to a string.
		[[nodiscard]] std::string IDToStr() const noexcept;

		[[nodiscard]] const Event* PopLatest() noexcept;
		[[nodiscard]] const Event* PopOldest() noexcept;

		[[nodiscard]] bool IsEventQueueEmpty() const noexcept;
	public:
		[[nodiscard]] inline ListenType Type() const noexcept { return m_Type; }
		[[nodiscard]] inline int ID() const noexcept { return m_ID; }
		[[nodiscard]] inline bool IsActive() const noexcept { return m_IsActive; }
		[[nodiscard]] inline bool IsNotified() const noexcept { return m_IsNotified; }
		[[nodiscard]] inline const std::deque<const Event*>& EventQueue() const noexcept { return m_EventQueue; }
	private:
		ListenType m_Type;
		int m_ID;
		bool m_IsActive;
		bool m_IsNotified;
		std::deque<const Event*> m_EventQueue; // NOTE: These pointers are non-owning, meaning they're only used as references to current events.
	};
}