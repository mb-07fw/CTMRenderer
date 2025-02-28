#include "Core/CorePCH.hpp"
#include "CTMRenderer/Event/EventPool.hpp"

namespace CTMRenderer::Event
{
	size_t EventPool::CountOf(ConcreteEventType type) const noexcept
	{
		if (!m_Pool.contains(type))
			return 0;

		return m_Pool.at(type).size();
	}
}