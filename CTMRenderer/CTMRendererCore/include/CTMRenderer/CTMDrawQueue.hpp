#pragma once

#include <queue>
#include <memory>

#include "CTMRenderer/CTMShape.hpp"

namespace CTMRenderer
{
	class CTMDrawQueue
	{
	public:
		CTMDrawQueue() = default;
		~CTMDrawQueue() = default;
	public:
		void QueueShape(const Shapes::CTMShape& shapeRef) noexcept;
	public:
		std::vector<const Shapes::CTMShape*> m_ShapeQueue;
	};
}