#include "Core/CorePCH.hpp"
#include "CTMRenderer/CTMDrawQueue.hpp"

namespace CTMRenderer
{
	void CTMDrawQueue::QueueShape(const Shapes::CTMShape& shapeRef) noexcept
	{
		m_ShapeQueue.emplace_back(&shapeRef);
	}
}