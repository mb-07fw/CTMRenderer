#pragma once

#include <unordered_map>
#include <vector>
#include <memory>

#include "Event/Event.hpp"

namespace CTMRenderer::Event
{
	class EventPool
	{
	public:
		EventPool() = default;
		~EventPool() = default;
	public:
		template <ConcreteEventType EnumConcreteTy, typename ConcreteEventTy, typename... Args>
		requires IsConcreteEventType<ConcreteEventTy>::Value && IsMatchingConcreteEventType<EnumConcreteTy, ConcreteEventTy>::Value
		inline ConcreteEventTy* PoolNew(Args&&... args) noexcept
		{
			std::vector<std::unique_ptr<IEvent>>& events = m_Pool[EnumConcreteTy];

			if (events.empty())
			{
				events.emplace_back(std::make_unique<ConcreteEventTy>(std::forward<Args>(args)...));
				return ConcreteEventTy::Cast(events.back().get());
			}
			
			ConcreteEventTy* pEvent = ConcreteEventTy::Cast(events.back().get());
			RUNTIME_ASSERT(pEvent != nullptr, "Invalid cast.\n");
			pEvent->Update(std::forward<Args>(args)...);
			return pEvent;
		}

		// Returns a non-owning pointer to the oldest event of EnumConcreteTy.
		// May return nullptr if no events are in the pool.
		template <ConcreteEventType EnumConcreteTy, typename ConcreteEventTy>
		requires IsConcreteEventType<ConcreteEventTy>::Value && IsMatchingConcreteEventType<EnumConcreteTy, ConcreteEventTy>::Value
		inline ConcreteEventTy* GetOldest() const noexcept
		{
			if (!m_Pool.contains(EnumConcreteTy))
				return nullptr;

			const std::vector<std::unique_ptr<IEvent>>& events = m_Pool.at(EnumConcreteTy);

			ConcreteEventTy* pEvent = ConcreteEventTy::Cast(events.back().get());
			RUNTIME_ASSERT(pEvent != nullptr, "Invalid cast.\n");
			return pEvent;
		}

		// Returns a non-owning pointer to the oldest event of EnumConcreteTy.
		// May return nullptr if no events are in the pool.
		template <ConcreteEventType EnumConcreteTy, typename ConcreteEventTy>
		requires IsConcreteEventType<ConcreteEventTy>::Value&& IsMatchingConcreteEventType<EnumConcreteTy, ConcreteEventTy>::Value
		inline ConcreteEventTy* GetNewest() const noexcept
		{
			if (!m_Pool.contains(EnumConcreteTy))
				return nullptr;

			const std::vector<std::unique_ptr<IEvent>>& events = m_Pool.at(EnumConcreteTy);

			ConcreteEventTy* pEvent = ConcreteEventTy::Cast(events.front().get());
			RUNTIME_ASSERT(pEvent != nullptr, "Invalid cast.\n");
			return pEvent;
		}
		
		inline size_t Count(ConcreteEventType type) const noexcept
		{
			if (!m_Pool.contains(type))
				return 0;

			return m_Pool.at(type).size();
		}
	private:
		std::unordered_map<ConcreteEventType, std::vector<std::unique_ptr<IEvent>>> m_Pool;
	};
}