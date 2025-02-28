#pragma once

#include "CTMRenderer/Event/EventDispatcher.hpp"

namespace CTMRenderer::Event
{
	class EventSystem
	{
	public:
		EventSystem() = default;
		~EventSystem() = default;
	public:
		inline [[nodiscard]] EventDispatcher& Dispatcher() noexcept { return m_Dispatcher; }
	private:
		EventDispatcher m_Dispatcher;
	};
}