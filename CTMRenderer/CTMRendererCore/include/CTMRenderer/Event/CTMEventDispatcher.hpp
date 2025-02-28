#pragma once

#include <vector>
#include <deque>
#include <atomic>

#include "CTMRenderer/Event/CTMEventListener.hpp"
#include "CTMRenderer/Event/CTMEventPool.hpp"
#include "Core/CoreMacros.hpp"

namespace CTMRenderer::Event
{
	class CTMEventDispatcher
	{
	public:
		CTMEventDispatcher() noexcept;
		~CTMEventDispatcher() = default;
	public:
		void Subscribe(ICTMGenericListener* pGenericListener) noexcept;
		void Subscribe(ICTMConcreteListener* pConcreteListener) noexcept;

		void Unsubscribe(ICTMGenericListener* pGenericListener) noexcept;
		void Unsubscribe(ICTMConcreteListener* pConcreteListener) noexcept;

		// Dispatches all queued events to all registered concrete and generic listeners.
		void DispatchQueued() noexcept;

		inline [[nodiscard]] bool IsEventQueued() const noexcept { return m_IsEventQueued.load(std::memory_order_acquire); }

		// Creates and queues a concrete event for dispatching. Queued events must be dispatched via `DispatchQueued()`
		// Requires ConcreteEventTy to be a concrete event type, like MouseMoveEvent.
		template <typename ConcreteEventTy, typename... Args>
			requires IsConcreteEventType<ConcreteEventTy>::Value
		inline void QueueEvent(Args&&... args) noexcept;

		// Returns a non-owning pointer to the oldest event of EnumConcreteTy.
		// May return nullptr if no events are in the pool.
		// Requires ConcreteEventTy to be a concrete event type, like MouseMoveEvent.
		template <typename ConcreteEventTy>
			requires IsConcreteEventType<ConcreteEventTy>::Value
		inline ConcreteEventTy* GetOldestEvent() noexcept;
	private:
		// Helper function to dispatch an event as a type.
		// Requires ConcreteEventTy to be a concrete event type, like MouseMoveEvent.
		template <typename ConcreteEventTy>
			requires IsConcreteEventType<ConcreteEventTy>::Value
		inline void DispatchEvent(ConcreteEventTy* pEvent) noexcept;

		// Dispatches an event to all registered IGenericListener's that listen for the event's GenericEventType, and any listeners that listen as CTM_ANY.
		// Requires ConcreteEventTy to be a concrete event type, like MouseMoveEvent.
		template <typename ConcreteEventTy>
			requires IsConcreteEventType<ConcreteEventTy>::Value
		inline void DispatchToGeneric(ConcreteEventTy* pEvent) const noexcept;

		// Dispatches an event to all registered IConcreteListener's that listen for the event's ConcreteEventType.
		// Requires ConcreteEventTy to be a concrete event type, like MouseMoveEvent.
		template <typename ConcreteEventTy>
			requires IsConcreteEventType<ConcreteEventTy>::Value
		inline void DispatchToConcrete(ConcreteEventTy* pEvent) const noexcept;
	private:
		CTMEventPool m_EventPool;
		std::unordered_map<CTMConcreteEventType, std::deque<ICTMEvent*>> m_EventQueues;
		std::atomic_bool m_IsEventQueued = false;
		std::unordered_map<CTMGenericEventType, std::vector<ICTMGenericListener*>> m_GenericListeners;
		std::unordered_map<CTMConcreteEventType, std::vector<ICTMConcreteListener*>> m_ConcreteListeners;
	};

	template <typename ConcreteEventTy, typename... Args>
		requires IsConcreteEventType<ConcreteEventTy>::Value
	inline void CTMEventDispatcher::QueueEvent(Args&&... args) noexcept
	{
		ConcreteEventTy* pEvent = m_EventPool.PoolNew<ConcreteEventTy::EnumConcreteTy, ConcreteEventTy>(std::forward<Args>(args)...);
		m_EventQueues[ConcreteEventTy::EnumConcreteTy].emplace_back(pEvent);

		m_IsEventQueued.store(true, std::memory_order_release);
	}

	template <typename ConcreteEventTy>
		requires IsConcreteEventType<ConcreteEventTy>::Value
	inline ConcreteEventTy* CTMEventDispatcher::GetOldestEvent() noexcept
	{
		return m_EventPool.GetOldest<ConcreteEventTy::EnumConcreteTy, ConcreteEventTy>();
	}

	template <typename ConcreteEventTy>
		requires IsConcreteEventType<ConcreteEventTy>::Value
	inline void CTMEventDispatcher::DispatchEvent(ConcreteEventTy* pEvent) noexcept
	{
		DispatchToGeneric(pEvent);
		DispatchToConcrete(pEvent);
	}

	template <typename ConcreteEventTy>
		requires IsConcreteEventType<ConcreteEventTy>::Value // Ensure the passed type (ConcreteEventTy) is a concrete event type.
	inline void CTMEventDispatcher::DispatchToGeneric(ConcreteEventTy* pEvent) const noexcept
	{
		RUNTIME_ASSERT(pEvent != nullptr, "Recieved event is nullptr.");

		// Get corresponding GenericEventType of the provided type.
		constexpr CTMGenericEventType EnunGenericType = EnumGenericEventTypeOf<ConcreteEventTy>::Type;
		RUNTIME_ASSERT(pEvent->GenericType() == EnunGenericType, "Mismatching GenericEventType's.\n");

		// TODO : Use .find to avoid duplicate loop-ups per GenericEventType. (contains() and at())
		if (m_GenericListeners.contains(EnunGenericType))
		{
			const std::vector<ICTMGenericListener*>& genericListeners = m_GenericListeners.at(EnunGenericType);
			for (ICTMGenericListener* pGenericListener : genericListeners)
				static_cast<CTMGenericListener<EnunGenericType>*>(pGenericListener)->Notify(pEvent);
		}

		// TODO : Use .find to avoid duplicate loop-ups per GenericEventType. (contains() and at())
		if (m_GenericListeners.contains(CTMGenericEventType::CTM_ANY))
		{
			const std::vector<ICTMGenericListener*>& ctmAnyListeners = m_GenericListeners.at(CTMGenericEventType::CTM_ANY);
			for (ICTMGenericListener* pAnyListener : ctmAnyListeners)
				static_cast<CTMGenericListener<CTMGenericEventType::CTM_ANY>*>(pAnyListener)->Notify(pEvent);
		}
	}

	template <typename ConcreteEventTy>
		requires IsConcreteEventType<ConcreteEventTy>::Value
	inline void CTMEventDispatcher::DispatchToConcrete(ConcreteEventTy* pEvent) const noexcept
	{
		RUNTIME_ASSERT(pEvent != nullptr, "Recieved concrete event is nullptr.");

		// Get corresponding ConcreteEventType of the provided type.
		constexpr CTMConcreteEventType EnumConcreteType = EnumConcreteEventTypeOf<ConcreteEventTy>::Type;
		RUNTIME_ASSERT(pEvent->ConcreteType() == EnumConcreteType, "Mismatching ConcreteEventType's.\n");

		if (!m_ConcreteListeners.contains(EnumConcreteType))
			return;

		const std::vector<ICTMConcreteListener*>& concreteListeners = m_ConcreteListeners.at(EnumConcreteType);
		for (ICTMConcreteListener* pConcreteListener : concreteListeners)
			static_cast<CTMConcreteListener<EnumConcreteType, ConcreteEventTy>*>(pConcreteListener)->Notify(pEvent);
	}
}