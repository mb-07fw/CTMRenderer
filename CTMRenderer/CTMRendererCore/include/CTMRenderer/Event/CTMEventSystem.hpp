#pragma once

#include "CTMRenderer/Event/CTMEventDispatcher.hpp"

namespace CTMRenderer::Event
{
	class CTMEventSystem
	{
	public:
		CTMEventSystem() = default;
		~CTMEventSystem() = default;
	public:
		inline [[nodiscard]] CTMEventDispatcher& Dispatcher() noexcept { return m_Dispatcher; }
	private:
		CTMEventDispatcher m_Dispatcher;
	};
}