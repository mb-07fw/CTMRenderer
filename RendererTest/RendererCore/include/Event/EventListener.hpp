#pragma once

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
		EventListener(ListenType listenType, uint32_t id);
		EventListener(const EventListener&) = delete;
		EventListener(EventListener&&) = delete;
		EventListener& operator=(const EventListener&) = delete;
		EventListener& operator=(EventListener&&) = delete;
	public:
		void ActivateListener() noexcept;
		void Notify(const Event* pEvent) noexcept;
		void Clear() noexcept;
	public:
		[[nodiscard]] inline ListenType Type()				const noexcept { return m_Type;		    }
		[[nodiscard]] inline int ID()						const noexcept { return m_ID;		    }
		[[nodiscard]] inline bool IsActive()				const noexcept { return m_IsActive;     }
		[[nodiscard]] inline bool IsNotified()				const noexcept { return m_IsNotified;   }
		[[nodiscard]] inline const Event* CurrentEvent()	const noexcept { return m_CurrentEvent; }
	public:
		[[nodiscard]] static ListenType GetListenTypeFromEvent(EventType eventType) noexcept;
	private:
		ListenType m_Type = ListenType::INVALID;
		int m_ID = -1;
		bool m_IsActive = false;
		bool m_IsNotified = false;
		const Event* m_CurrentEvent = nullptr;
	};
}