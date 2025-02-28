#pragma once

#include <unordered_map>
#include <vector>
#include <memory>

#include "CTMRenderer/Event/Event.hpp"

namespace CTMRenderer::Event
{
	class EventPool
	{
	public:
		EventPool() = default;
		~EventPool() = default;
	public:
		// Creates a new event of type ConcreteEventTy and pools it, or returns an already pooled event of the specified EnumConcreteTy.
		// Returns a pointer to the pooled event.
		template <ConcreteEventType EnumConcreteTy, typename ConcreteEventTy, typename... Args>
			requires IsConcreteEventType<ConcreteEventTy>::Value && IsMatchingConcreteEventType<EnumConcreteTy, ConcreteEventTy>::Value
		ConcreteEventTy* PoolNew(Args&&... args) noexcept;

		// Returns a non-owning pointer to the oldest event of ConcreteEventTy.
		// May return nullptr if no events are in the pool.
		template <ConcreteEventType EnumConcreteTy, typename ConcreteEventTy>
			requires IsConcreteEventType<ConcreteEventTy>::Value && IsMatchingConcreteEventType<EnumConcreteTy, ConcreteEventTy>::Value
		inline ConcreteEventTy* GetOldest() const noexcept;

		// Returns a non-owning pointer to the newest event of EnumConcreteTy.
		// May return nullptr if no events are in the pool.
		template <ConcreteEventType EnumConcreteTy, typename ConcreteEventTy>
			requires IsConcreteEventType<ConcreteEventTy>::Value&& IsMatchingConcreteEventType<EnumConcreteTy, ConcreteEventTy>::Value
		inline ConcreteEventTy* GetNewest() const noexcept;
		
		// Returns the number of pooled events mapped to the ConcreteEventType.
		size_t CountOf(ConcreteEventType type) const noexcept;
	private:
		std::unordered_map<ConcreteEventType, std::vector<std::unique_ptr<IEvent>>> m_Pool;
	};



#pragma region Template Implementations
	template <ConcreteEventType EnumConcreteTy, typename ConcreteEventTy, typename... Args>
		requires IsConcreteEventType<ConcreteEventTy>::Value&& IsMatchingConcreteEventType<EnumConcreteTy, ConcreteEventTy>::Value
	inline ConcreteEventTy* EventPool::PoolNew(Args&&... args) noexcept
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
	
	template <ConcreteEventType EnumConcreteTy, typename ConcreteEventTy>
		requires IsConcreteEventType<ConcreteEventTy>::Value&& IsMatchingConcreteEventType<EnumConcreteTy, ConcreteEventTy>::Value
	inline ConcreteEventTy* EventPool::GetOldest() const noexcept
	{
		if (!m_Pool.contains(EnumConcreteTy))
			return nullptr;

		const std::vector<std::unique_ptr<IEvent>>& events = m_Pool.at(EnumConcreteTy);

		ConcreteEventTy* pEvent = ConcreteEventTy::Cast(events.back().get());
		RUNTIME_ASSERT(pEvent != nullptr, "Invalid cast.\n");
		return pEvent;
	}

	template <ConcreteEventType EnumConcreteTy, typename ConcreteEventTy>
		requires IsConcreteEventType<ConcreteEventTy>::Value&& IsMatchingConcreteEventType<EnumConcreteTy, ConcreteEventTy>::Value
	inline ConcreteEventTy* EventPool::GetNewest() const noexcept
	{
		if (!m_Pool.contains(EnumConcreteTy))
			return nullptr;

		const std::vector<std::unique_ptr<IEvent>>& events = m_Pool.at(EnumConcreteTy);

		ConcreteEventTy* pEvent = ConcreteEventTy::Cast(events.front().get());
		RUNTIME_ASSERT(pEvent != nullptr, "Invalid cast.\n");
		return pEvent;
	}
#pragma endregion
}