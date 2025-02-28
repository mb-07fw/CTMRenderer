#include "Core/CorePCH.hpp"
#include "CTMRenderer/Event/CTMEventPool.hpp"

namespace CTMRenderer::Event
{
	size_t CTMEventPool::CountOf(CTMConcreteEventType concreteType) const noexcept
	{
		if (!m_Pool.contains(concreteType))
			return 0;

		return m_Pool.at(concreteType).size();
	}
}