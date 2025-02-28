#pragma once

#include <vector>
#include <deque>
#include <atomic>

#include "CTMRenderer/Event/EventListener.hpp"
#include "CTMRenderer/Event/EventPool.hpp"
#include "Core/CoreMacros.hpp"

namespace CTMRenderer::Event
{
	class EventDispatcher
	{
	public:
		EventDispatcher() noexcept;
		~EventDispatcher() = default;
	public:
		void Subscribe(IGenericListener* pGenericListener) noexcept;
		void Subscribe(IConcreteListener* pConcreteListener) noexcept;

		void Unsubscribe(IGenericListener* pGenericListener) noexcept;
		void Unsubscribe(IConcreteListener* pConcreteListener) noexcept;

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
		EventPool m_EventPool;
		std::unordered_map<ConcreteEventType, std::deque<IEvent*>> m_EventQueues;
		std::atomic_bool m_IsEventQueued = false;
		std::unordered_map<GenericEventType, std::vector<IGenericListener*>> m_GenericListeners;
		std::unordered_map<ConcreteEventType, std::vector<IConcreteListener*>> m_ConcreteListeners;
	};

	template <typename ConcreteEventTy, typename... Args>
		requires IsConcreteEventType<ConcreteEventTy>::Value
	inline void EventDispatcher::QueueEvent(Args&&... args) noexcept
	{
		ConcreteEventTy* pEvent = m_EventPool.PoolNew<ConcreteEventTy::EnumConcreteTy, ConcreteEventTy>(std::forward<Args>(args)...);
		m_EventQueues[ConcreteEventTy::EnumConcreteTy].emplace_back(pEvent);

		m_IsEventQueued.store(true, std::memory_order_release);
	}

	template <typename ConcreteEventTy>
		requires IsConcreteEventType<ConcreteEventTy>::Value
	inline ConcreteEventTy* EventDispatcher::GetOldestEvent() noexcept
	{
		return m_EventPool.GetOldest<ConcreteEventTy::EnumConcreteTy, ConcreteEventTy>();
	}

	template <typename ConcreteEventTy>
		requires IsConcreteEventType<ConcreteEventTy>::Value
	inline void EventDispatcher::DispatchEvent(ConcreteEventTy* pEvent) noexcept
	{
		DispatchToGeneric(pEvent);
		DispatchToConcrete(pEvent);
	}

	template <typename ConcreteEventTy>
		requires IsConcreteEventType<ConcreteEventTy>::Value // Ensure the passed type (ConcreteEventTy) is a concrete event type.
	inline void EventDispatcher::DispatchToGeneric(ConcreteEventTy* pEvent) const noexcept
	{
		RUNTIME_ASSERT(pEvent != nullptr, "Recieved event is nullptr.");

		// Get corresponding GenericEventType of the provided type.
		constexpr GenericEventType EnunGenericType = EnumGenericEventTypeOf<ConcreteEventTy>::Type;
		RUNTIME_ASSERT(pEvent->GenericType() == EnunGenericType, "Mismatching GenericEventType's.\n");

		// TODO : Use .find to avoid duplicate loop-ups per GenericEventType. (contains() and at())
		if (m_GenericListeners.contains(EnunGenericType))
		{
			const std::vector<IGenericListener*>& genericListeners = m_GenericListeners.at(EnunGenericType);
			for (IGenericListener* pGenericListener : genericListeners)
				static_cast<GenericListener<EnunGenericType>*>(pGenericListener)->Notify(pEvent);
		}

		// TODO : Use .find to avoid duplicate loop-ups per GenericEventType. (contains() and at())
		if (m_GenericListeners.contains(GenericEventType::CTM_ANY))
		{
			const std::vector<IGenericListener*>& ctmAnyListeners = m_GenericListeners.at(GenericEventType::CTM_ANY);
			for (IGenericListener* pAnyListener : ctmAnyListeners)
				static_cast<GenericListener<GenericEventType::CTM_ANY>*>(pAnyListener)->Notify(pEvent);
		}
	}

	template <typename ConcreteEventTy>
		requires IsConcreteEventType<ConcreteEventTy>::Value
	inline void EventDispatcher::DispatchToConcrete(ConcreteEventTy* pEvent) const noexcept
	{
		RUNTIME_ASSERT(pEvent != nullptr, "Recieved concrete event is nullptr.");

		// Get corresponding ConcreteEventType of the provided type.
		constexpr ConcreteEventType EnumConcreteType = EnumConcreteEventTypeOf<ConcreteEventTy>::Type;
		RUNTIME_ASSERT(pEvent->ConcreteType() == EnumConcreteType, "Mismatching ConcreteEventType's.\n");

		if (!m_ConcreteListeners.contains(EnumConcreteType))
			return;

		const std::vector<IConcreteListener*>& concreteListeners = m_ConcreteListeners.at(EnumConcreteType);
		for (IConcreteListener* pConcreteListener : concreteListeners)
			static_cast<ConcreteListener<EnumConcreteType, ConcreteEventTy>*>(pConcreteListener)->Notify(pEvent);
	}
}