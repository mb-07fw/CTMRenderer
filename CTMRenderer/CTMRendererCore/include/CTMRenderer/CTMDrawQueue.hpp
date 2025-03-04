#pragma once

#include <queue>
#include <memory>
#include <vector>
//#include <mutex>

#include "CTMRenderer/CTMShape.hpp"

namespace CTMRenderer
{
	class CTMDrawQueue
	{
	public:
		CTMDrawQueue() = default;
		~CTMDrawQueue() = default;
	public:
		void QueueShape(const Shapes::CTMRect& shapeRef) noexcept;
		//void LockQueue() noexcept;
		//void UnlockQueue() noexcept;

		[[nodiscard]] std::vector<const Shapes::CTMRect*>& RectQueue() noexcept { return m_RectQueue; }
	private:
		//std::mutex m_Mutex;
		//std::unique_lock<std::mutex> m_Lock;
		std::vector<const Shapes::CTMRect*> m_RectQueue;
	};
}