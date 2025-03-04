#include "Core/CorePCH.hpp"
#include "CTMRenderer/CTMDrawQueue.hpp"

namespace CTMRenderer
{
	void CTMDrawQueue::QueueShape(const Shapes::CTMRect& shapeRef) noexcept
	{
		m_RectQueue.emplace_back(&shapeRef);
	}
}