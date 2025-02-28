#pragma once

#include <unordered_map>
#include <vector>
#include <memory>

#include "CTMRenderer/Event/CTMEvent.hpp"

namespace CTMRenderer::Event
{
	class CTMEventPool
	{
	public:
		CTMEventPool() = default;
		~CTMEventPool() = default;
	public:
		// Creates a new event of type ConcreteEventTy and pools it, or returns an already pooled event of the specified EnumConcreteTy.
		// Returns a pointer to the pooled event.
		template <CTMConcreteEventType EnumConcreteTy, typename ConcreteEventTy, typename... Args>
			requires IsConcreteEventType<ConcreteEventTy>::Value && IsMatchingConcreteEventType<EnumConcreteTy, ConcreteEventTy>::Value
		ConcreteEventTy* PoolNew(Args&&... args) noexcept;

		// Returns a non-owning pointer to the oldest event of ConcreteEventTy.
		// May return nullptr if no events are in the pool.
		template <CTMConcreteEventType EnumConcreteTy, typename ConcreteEventTy>
			requires IsConcreteEventType<ConcreteEventTy>::Value && IsMatchingConcreteEventType<EnumConcreteTy, ConcreteEventTy>::Value
		inline ConcreteEventTy* GetOldest() const noexcept;

		// Returns a non-owning pointer to the newest event of EnumConcreteTy.
		// May return nullptr if no events are in the pool.
		template <CTMConcreteEventType EnumConcreteTy, typename ConcreteEventTy>
			requires IsConcreteEventType<ConcreteEventTy>::Value&& IsMatchingConcreteEventType<EnumConcreteTy, ConcreteEventTy>::Value
		inline ConcreteEventTy* GetNewest() const noexcept;
		
		// Returns the number of pooled events mapped to the ConcreteEventType.
		size_t CountOf(CTMConcreteEventType concreteType) const noexcept;
	private:
		std::unordered_map<CTMConcreteEventType, std::vector<std::unique_ptr<ICTMEvent>>> m_Pool;
	};



#pragma region Template Implementations
	template <CTMConcreteEventType EnumConcreteTy, typename ConcreteEventTy, typename... Args>
		requires IsConcreteEventType<ConcreteEventTy>::Value&& IsMatchingConcreteEventType<EnumConcreteTy, ConcreteEventTy>::Value
	inline ConcreteEventTy* CTMEventPool::PoolNew(Args&&... args) noexcept
	{
		std::vector<std::unique_ptr<ICTMEvent>>& events = m_Pool[EnumConcreteTy];

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
	
	template <CTMConcreteEventType EnumConcreteTy, typename ConcreteEventTy>
		requires IsConcreteEventType<ConcreteEventTy>::Value&& IsMatchingConcreteEventType<EnumConcreteTy, ConcreteEventTy>::Value
	inline ConcreteEventTy* CTMEventPool::GetOldest() const noexcept
	{
		if (!m_Pool.contains(EnumConcreteTy))
			return nullptr;

		const std::vector<std::unique_ptr<ICTMEvent>>& events = m_Pool.at(EnumConcreteTy);

		ConcreteEventTy* pEvent = ConcreteEventTy::Cast(events.back().get());
		RUNTIME_ASSERT(pEvent != nullptr, "Invalid cast.\n");
		return pEvent;
	}

	template <CTMConcreteEventType EnumConcreteTy, typename ConcreteEventTy>
		requires IsConcreteEventType<ConcreteEventTy>::Value&& IsMatchingConcreteEventType<EnumConcreteTy, ConcreteEventTy>::Value
	inline ConcreteEventTy* CTMEventPool::GetNewest() const noexcept
	{
		if (!m_Pool.contains(EnumConcreteTy))
			return nullptr;

		const std::vector<std::unique_ptr<ICTMEvent>>& events = m_Pool.at(EnumConcreteTy);

		ConcreteEventTy* pEvent = ConcreteEventTy::Cast(events.front().get());
		RUNTIME_ASSERT(pEvent != nullptr, "Invalid cast.\n");
		return pEvent;
	}
#pragma endregion
}