#pragma once

#include <functional>

#include "CTMRenderer/Event/CTMEvent.hpp"
#include "Core/CoreMacros.hpp"

namespace CTMRenderer::Event
{
#pragma region Listener Interfaces
	class ICTMListener
	{
		friend class CTMEventDispatcher;
	public:
		ICTMListener(bool listensAbstract) noexcept;
		virtual ~ICTMListener() = default;
	public:
		inline [[nodiscard]] bool ListensAbstract() const noexcept { return m_ListensAbstract; }
		inline [[nodiscard]] bool ListensConcrete() const noexcept { return m_ListensConcrete; }
	protected:
		inline void Register() noexcept { m_IsRegistered = true; }
		inline void Unegister() noexcept { m_IsRegistered = false; }
		inline constexpr [[nodiscard]] bool IsRegistered() const noexcept { return m_IsRegistered; }
	protected:
		bool m_IsRegistered = false;
		bool m_ListensAbstract = false;
		bool m_ListensConcrete = false;
	};



	class ICTMGenericListener : public ICTMListener
	{
	public:
		ICTMGenericListener(CTMGenericEventType listenType) noexcept;
		virtual ~ICTMGenericListener() = default;

		virtual [[nodiscard]] CTMGenericEventType ListenType() const noexcept { return m_ListenType; }
	private:
		CTMGenericEventType m_ListenType;
	};



	class ICTMConcreteListener : public ICTMListener
	{
	public:
		ICTMConcreteListener(CTMConcreteEventType listenType) noexcept;
		virtual ~ICTMConcreteListener() = default;

		virtual [[nodiscard]] CTMConcreteEventType ListenType() const noexcept { return m_ListenType; }
	private:
		CTMConcreteEventType m_ListenType;
	};
#pragma endregion



#pragma region Listener Types
	template <CTMGenericEventType EnumGenericTy>
	class CTMGenericListener : public ICTMGenericListener
	{
	public:
		inline CTMGenericListener(const std::function<void(ICTMEvent*)>& onNotifyFunc) noexcept;
		~CTMGenericListener() = default;

		void Notify(Event::ICTMEvent* pEvent) noexcept;
	private:
		std::function<void(ICTMEvent*)> m_OnNotifyFunc;
	};



	template <CTMConcreteEventType EnumConcreteTy, typename ConcreteEventTy>
		requires IsMatchingConcreteEventType<EnumConcreteTy, ConcreteEventTy>::Value
	class CTMConcreteListener : public ICTMConcreteListener
	{
	public:
		inline CTMConcreteListener(const std::function<void(ConcreteEventTy*)>& onNotifyFunc) noexcept;
		~CTMConcreteListener() = default;

		void Notify(ConcreteEventTy* pEvent) noexcept;
 	private:
		std::function<void(ConcreteEventTy*)> m_OnNotifyFunc;
	};
#pragma endregion



#pragma region Template Implementations
	template <CTMGenericEventType EnumGenericTy>
	inline CTMGenericListener<EnumGenericTy>::CTMGenericListener(const std::function<void(ICTMEvent*)>& onNotifyFunc) noexcept
		: ICTMGenericListener(EnumGenericTy), m_OnNotifyFunc(onNotifyFunc)
	{
	}

	template <CTMGenericEventType EnumGenericTy>
	inline void CTMGenericListener<EnumGenericTy>::Notify(Event::ICTMEvent* pEvent) noexcept
	{
		RUNTIME_ASSERT(pEvent != nullptr, "Received event was nullptr.\n");
		m_OnNotifyFunc(pEvent);
	}

	template <CTMConcreteEventType EnumConcreteTy, typename ConcreteEventTy>
		requires IsMatchingConcreteEventType<EnumConcreteTy, ConcreteEventTy>::Value
	inline CTMConcreteListener<EnumConcreteTy, ConcreteEventTy>::CTMConcreteListener(const std::function<void(ConcreteEventTy*)>& onNotifyFunc) noexcept
			: ICTMConcreteListener(EnumConcreteTy), m_OnNotifyFunc(onNotifyFunc)
	{
	}

	template <CTMConcreteEventType EnumConcreteTy, typename ConcreteEventTy>
		requires IsMatchingConcreteEventType<EnumConcreteTy, ConcreteEventTy>::Value
	inline void CTMConcreteListener<EnumConcreteTy, ConcreteEventTy>::Notify(ConcreteEventTy* pEvent) noexcept
	{
		RUNTIME_ASSERT(pEvent != nullptr, "Received event was nullptr.\n");
		m_OnNotifyFunc(pEvent);
	}
#pragma endregion
}