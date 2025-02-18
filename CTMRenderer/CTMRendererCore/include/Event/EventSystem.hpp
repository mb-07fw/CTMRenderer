#pragma once

#include "Event/EventDispatcher.hpp"

namespace CTMRenderer::Event
{
	class EventSystem
	{
	public:
		EventSystem() noexcept;
		~EventSystem() = default;
	public:
		inline [[nodiscard]] EventDispatcher& Dispatcher() noexcept { return m_Dispatcher; }
	private:
		EventDispatcher m_Dispatcher;
	};
}