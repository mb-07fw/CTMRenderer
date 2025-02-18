#pragma once

#include <functional>

#include "Event/Event.hpp"
#include "Core/CoreMacros.hpp"

namespace CTMRenderer::Event
{
	class IListener
	{
		friend class EventDispatcher;
	public:
		virtual ~IListener() = default;
		virtual [[nodiscard]] bool ListensAbstract() const noexcept = 0;
		virtual [[nodiscard]] bool ListensConcrete() const noexcept = 0;
	protected:
		inline void Register() noexcept { m_IsRegistered = true; }
		inline void Unegister() noexcept { m_IsRegistered = false; }
		inline constexpr [[nodiscard]] bool IsRegistered() const noexcept { return m_IsRegistered; }
	protected:
		bool m_IsRegistered = false;
	};

	class IGenericListener : public IListener
	{
	public:
		virtual ~IGenericListener() = default;
		virtual [[nodiscard]] GenericEventType ListenType() const noexcept = 0;
	};

	class IConcreteListener : public IListener
	{
	public:
		virtual ~IConcreteListener() = default;
		virtual [[nodiscard]] ConcreteEventType ListenType() const noexcept = 0;
	};

	// TODO: Use a template-based callable.
	template <typename NotifyFuncArgs, typename EventNotifyArgs>
	class Listener : public virtual IListener // Inherit IListener virtually, as both IGenericListener and IConcreteListener already construct IListener implicitly.
	{
	public:
		inline Listener(std::function<NotifyFuncArgs>& onNotifyFunc) noexcept
			: m_OnNotifyFunc(onNotifyFunc) {}

		virtual ~Listener() = default;
	public:
		inline void Notify(EventNotifyArgs pEvent) noexcept
		{
			RUNTIME_ASSERT(pEvent != nullptr, "Recieved event is nullptr.\n");

			m_OnNotifyFunc(pEvent);
		}
	private:
		std::function<NotifyFuncArgs> m_OnNotifyFunc;
	};

	template <GenericEventType EnumGenericTy>
	class GenericListener : public Listener<void(IEvent*), IEvent*>, public IGenericListener
	{
	public:
		inline GenericListener(std::function<void(IEvent*)>& onNotifyFunc) noexcept
			: Listener<void(IEvent*), IEvent*>(onNotifyFunc) {}
		
		// Secondary value constructor for lambda's.
		inline GenericListener(std::function<void(IEvent*)> onNotifyFunc) noexcept
			: Listener<void(IEvent*), IEvent*>(onNotifyFunc) {}

		~GenericListener() = default;
	public:
		inline virtual constexpr [[nodiscard]] bool ListensAbstract() const noexcept override { return true; }
		inline virtual constexpr [[nodiscard]] bool ListensConcrete() const noexcept override { return false; }
		inline virtual constexpr GenericEventType ListenType() const noexcept override { return EnumGenericTy; }
	};

	template <ConcreteEventType EnumConcreteTy, typename ConcreteEventTy>
	requires IsMatchingConcreteEventType<EnumConcreteTy, ConcreteEventTy>::Value // Ensure the passed ConcreteEventType (EnumConcreteTy) and ConcreteEventTy match.
	class ConcreteListener : public Listener<void(ConcreteEventTy*), ConcreteEventTy*>, public IConcreteListener
	{
	public:
		inline ConcreteListener(std::function<void(ConcreteEventTy*)>& onNotifyFunc) noexcept
			: Listener(onNotifyFunc) {}

		// Secondary value constructor for lambda's.
		inline ConcreteListener(std::function<void(ConcreteEventTy*)> onNotifyFunc) noexcept
			: Listener<void(ConcreteEventTy*), ConcreteEventTy*>(onNotifyFunc) {}

		~ConcreteListener() = default;
	public:
		inline virtual constexpr [[nodiscard]] bool ListensAbstract() const noexcept override { return false; }
		inline virtual constexpr [[nodiscard]] bool ListensConcrete() const noexcept override { return true; }
		inline virtual constexpr ConcreteEventType ListenType() const noexcept override { return EnumConcreteTy; }
	};
}