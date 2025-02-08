#pragma once

#include <memory>
#include <functional>
#include <vector>

#include "Event/EventListener.hpp"
#include "Event/EventPool.hpp"
#include "Core/CoreMacros.hpp"

namespace CTMRenderer::Event
{
	class EventDispatcher
	{
	public:
		EventDispatcher() noexcept;
		~EventDispatcher() = default;
	public:
		void Subscribe(IGenericListener* genericListener) noexcept;
		void Subscribe(IConcreteListener* concreteListener) noexcept;

		void Unsubscribe(IGenericListener* genericListener) noexcept;
		void Unsubscribe(IConcreteListener* concreteListener) noexcept;

		// Dispatches a concrete event type to all registered listeners that listen for it.
		// Requires ConcreteEventTy to be a concrete event type, like MouseMoveEvent.
		template <typename ConcreteEventTy, typename... Args>
		requires IsConcreteEventType<ConcreteEventTy>::Value // Ensure the passed type (ConcreteEventTy) is a concrete event type like MouseMoveEvent.
		inline void Dispatch(Args&&... args) noexcept
		{
			ConcreteEventTy* pEvent = m_EventPool.PoolNew<ConcreteEventTy::EnumConcreteTy, ConcreteEventTy>(std::forward<Args>(args)...);
			
			DispatchToGeneric(pEvent);
			DispatchToConcrete(pEvent);
		}

		// Dispatches the newest event of the provided ConcreteEventTy that was previously pooled.
		// Requires ConcreteEventTy to be a concrete event type, like MouseMoveEvent.
		template <ConcreteEventType EnumConcreteTy, typename ConcreteEventTy>
		requires IsConcreteEventType<ConcreteEventTy>::Value&& IsMatchingConcreteEventType<EnumConcreteTy, ConcreteEventTy>::Value
		inline void DispatchExisting() const noexcept
		{
			ConcreteEventTy* pEvent = m_EventPool.GetNewest<EnumConcreteTy, ConcreteEventTy>();

			if (pEvent == nullptr)
				return;

			DispatchToGeneric(pEvent);
			DispatchToConcrete(pEvent);
		}

		// Returns a non-owning pointer to the oldest event of EnumConcreteTy.
		// May return nullptr if no events are in the pool.
		template <typename ConcreteEventTy>
		requires IsConcreteEventType<ConcreteEventTy>::Value
		inline ConcreteEventTy* GetOldestEvent() noexcept
		{
			return m_EventPool.GetOldest<ConcreteEventTy::EnumConcreteTy, ConcreteEventTy>();
		}
	private:
		// Dispatches an event to all registered IGenericListener's that listen for the event's GenericEventType, and any listeners that listen as CTM_ANY.
		// Requires EventTy to be a concrete event type, like MouseMoveEvent.
		template <typename ConcreteEventTy>
		requires IsConcreteEventType<ConcreteEventTy>::Value // Ensure the passed type (ConcreteEventTy) is a concrete event type.
		inline void DispatchToGeneric(ConcreteEventTy* pEvent) const noexcept
		{
			RUNTIME_ASSERT(pEvent != nullptr, "Recieved event is nullptr.");

			// Get corresponding GenericEventType of the provided type.
			constexpr GenericEventType EnunGenericType = EnumGenericEventTypeOf<ConcreteEventTy>::Type;

			// TODO : Use .find to avoid duplicate loop-ups per event dispatch.
			if (m_GenericListeners.contains(EnunGenericType))
			{
				const std::vector<IGenericListener*>& genericListeners = m_GenericListeners.at(EnunGenericType);
				for (IGenericListener* pGenericListener : genericListeners)
					static_cast<GenericListener<EnunGenericType>*>(pGenericListener)->Notify(pEvent);
			}

			if (m_GenericListeners.contains(GenericEventType::CTM_ANY))
			{
				const std::vector<IGenericListener*>& ctmAnyListeners = m_GenericListeners.at(GenericEventType::CTM_ANY);
				for (IGenericListener* pAnyListener : ctmAnyListeners)
					static_cast<GenericListener<GenericEventType::CTM_ANY>*>(pAnyListener)->Notify(pEvent);
			}
		}

		// Dispatches an event to all registered IConcreteListener's that listen for the event's ConcreteEventType.
		// Requires EventTy to be a concrete event type, like MouseMoveEvent.
		template <typename ConcreteEventTy>
		requires IsConcreteEventType<ConcreteEventTy>::Value // Ensure the passed type (ConcreteEventTy) is a concrete event type.
		inline void DispatchToConcrete(ConcreteEventTy* pEvent) const noexcept
		{
			RUNTIME_ASSERT(pEvent != nullptr, "Recieved concrete event is nullptr.");

			// Get corresponding ConcreteEventType of the provided type.
			constexpr ConcreteEventType EnumConcreteType = EnumConcreteEventTypeOf<ConcreteEventTy>::Type;

			if (!m_ConcreteListeners.contains(EnumConcreteType))
				return;
			
			const std::vector<IConcreteListener*>& concreteListeners = m_ConcreteListeners.at(EnumConcreteType);
			for (IConcreteListener* pConcreteListener : concreteListeners)
				static_cast<ConcreteListener<EnumConcreteType, ConcreteEventTy>*>(pConcreteListener)->Notify(pEvent);
		}
	private:
		EventPool m_EventPool;
		std::unordered_map<GenericEventType, std::vector<IGenericListener*>> m_GenericListeners;
		std::unordered_map<ConcreteEventType, std::vector<IConcreteListener*>> m_ConcreteListeners;
	};
}