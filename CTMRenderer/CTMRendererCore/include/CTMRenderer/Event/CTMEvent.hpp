#pragma once

#include <string_view>

#include "Core/CoreMacros.hpp"

namespace CTMRenderer::Event
{
	enum class CTMConcreteEventType
	{
		CTM_STATE_START_EVENT,
		CTM_STATE_END_EVENT,

		CTM_MOUSE_MOVE_EVENT,

		CTM_FRAME_CLEAR_FRAME_EVENT,
		CTM_FRAME_START_FRAME_EVENT,
		CTM_FRAME_DRAW_FRAME_EVENT,
	};



	enum class CTMGenericEventType
	{
		CTM_ANY, // Used to listen for any event type via the dispatcher.
		CTM_STATE_EVENT,
		CTM_MOUSE_EVENT,
		CTM_FRAME_EVENT
	};



	class ICTMEvent
	{
	public:
		ICTMEvent(CTMConcreteEventType concreteType, CTMGenericEventType genericType) noexcept;
		virtual ~ICTMEvent() = default;
	public:
		[[nodiscard]] std::string_view GenericTypeToStr() const noexcept;
		[[nodiscard]] std::string_view ConcreteTypeToStr() const noexcept;
		
		inline [[nodiscard]] CTMConcreteEventType ConcreteType() const noexcept { return m_ConcreteType; }
		inline [[nodiscard]] CTMGenericEventType GenericType() const noexcept { return m_GenericType; }
	private:
		CTMConcreteEventType m_ConcreteType;
		CTMGenericEventType m_GenericType;
	};



	template <typename DerivedTy, CTMConcreteEventType EnumConcreteTy, CTMGenericEventType EnumGenericTy>
	class CTMEvent : public ICTMEvent
	{
	public:
		static constexpr CTMConcreteEventType EnumConcreteTy = EnumConcreteTy;
		static constexpr CTMGenericEventType EnumGenericTy = EnumGenericTy;
	public:
		CTMEvent() noexcept;
		virtual ~CTMEvent() = default;
	public:
		static constexpr [[nodiscard]] bool IsInstanceOf(const ICTMEvent* pEvent) noexcept;
		static constexpr [[nodiscard]] DerivedTy* Cast(ICTMEvent* pEvent) noexcept;
	};



#pragma region StateEvent
	template <typename DerivedTy, CTMConcreteEventType EnumConcreteTy>
	class CTMStateEvent : public CTMEvent<DerivedTy, EnumConcreteTy, CTMGenericEventType::CTM_STATE_EVENT>
	{
	public:
		CTMStateEvent() = default;
		virtual ~CTMStateEvent() = default;
	};



	class CTMStartEvent : public CTMStateEvent<CTMStartEvent, CTMConcreteEventType::CTM_STATE_START_EVENT>
	{
	public:
		CTMStartEvent(unsigned int placeholderArgs) noexcept;
		~CTMStartEvent() = default;
	public:
		void Update(unsigned int newPlaceholderArgs) noexcept;
		inline [[nodiscard]] unsigned int PlaceholderArgs() const noexcept { return m_PlaceholderArgs; }
	private:
		unsigned int m_PlaceholderArgs;
	};



	class CTMEndEvent : public CTMStateEvent<CTMEndEvent, CTMConcreteEventType::CTM_STATE_END_EVENT>
	{
	public:
		CTMEndEvent(unsigned int placeholderArgs) noexcept;
		~CTMEndEvent() = default;
	public:
		void Update(unsigned int newPlaceholderArgs) noexcept;
		inline [[nodiscard]] unsigned int PlaceholderArgs() const noexcept { return m_PlaceholderArgs; }
	private:
		unsigned int m_PlaceholderArgs;
	};
#pragma endregion



#pragma region Mouse Events
	template <typename DerivedTy, CTMConcreteEventType EnumConcreteTy>
	class CTMMouseEvent : public CTMEvent<DerivedTy, EnumConcreteTy, CTMGenericEventType::CTM_MOUSE_EVENT>
	{
	public:
		CTMMouseEvent(unsigned int posX, unsigned int posY) noexcept;
		virtual ~CTMMouseEvent() = default;
	public:
		inline [[nodiscard]] unsigned int PosX() const noexcept { return m_PosX; }
		inline [[nodiscard]] unsigned int PosY() const noexcept { return m_PosY; }
	protected:
		unsigned int m_PosX;
		unsigned int m_PosY;
	};



	class CTMMouseMoveEvent : public CTMMouseEvent<CTMMouseMoveEvent, CTMConcreteEventType::CTM_MOUSE_MOVE_EVENT>
	{
	public:
		CTMMouseMoveEvent(unsigned int posX, unsigned int posY) noexcept;
	public:
		void Update(unsigned int newPosX, unsigned int newPosY) noexcept;
	};
#pragma endregion



#pragma region Frame Events
	template <typename DerivedTy, CTMConcreteEventType EnumConcreteTy>
	class CTMFrameEvent : public CTMEvent<DerivedTy, EnumConcreteTy, CTMGenericEventType::CTM_FRAME_EVENT>
	{
	public:
		CTMFrameEvent() = default;
		virtual ~CTMFrameEvent() = default;
	};



	class CTMClearFrameEvent : public CTMFrameEvent<CTMClearFrameEvent, CTMConcreteEventType::CTM_FRAME_CLEAR_FRAME_EVENT>
	{
	public:
		CTMClearFrameEvent() = default;
		~CTMClearFrameEvent() = default;

		inline void Update() const noexcept {}
	};



	class CTMStartFrameEvent : public CTMFrameEvent<CTMStartFrameEvent, CTMConcreteEventType::CTM_FRAME_START_FRAME_EVENT>
	{
	public:
		CTMStartFrameEvent() = default;
		~CTMStartFrameEvent() = default;

		inline void Update() const noexcept {}
	};



	class CTMDrawFrameEvent : public CTMFrameEvent<CTMDrawFrameEvent, CTMConcreteEventType::CTM_FRAME_DRAW_FRAME_EVENT>
	{
	public:
		CTMDrawFrameEvent() = default;
		~CTMDrawFrameEvent() = default;

		inline void Update() const noexcept {}
	};
#pragma endregion



#pragma region Traits
	// Holds a false bool value.
	struct FalseValue {
		static constexpr bool Value = false;
	};



	// Holds a true bool value.
	struct TrueValue {
		static constexpr bool Value = true;
	};



#pragma region IsMatchingConcreteEventType
	template <CTMConcreteEventType EnumConcreteTy, typename ConcreteEventTy>
	struct IsMatchingConcreteEventType : public FalseValue {};

	template <>
	struct IsMatchingConcreteEventType<CTMConcreteEventType::CTM_STATE_START_EVENT, CTMStartEvent> : public TrueValue {};

	template <>
	struct IsMatchingConcreteEventType<CTMConcreteEventType::CTM_STATE_END_EVENT, CTMEndEvent> : public TrueValue {};



	template <>
	struct IsMatchingConcreteEventType<CTMConcreteEventType::CTM_MOUSE_MOVE_EVENT, CTMMouseMoveEvent> : public TrueValue {};



	template <>
	struct IsMatchingConcreteEventType<CTMConcreteEventType::CTM_FRAME_CLEAR_FRAME_EVENT, CTMClearFrameEvent> : public TrueValue {};

	template <>
	struct IsMatchingConcreteEventType<CTMConcreteEventType::CTM_FRAME_START_FRAME_EVENT, CTMStartFrameEvent> : public TrueValue {};

	template <>
	struct IsMatchingConcreteEventType<CTMConcreteEventType::CTM_FRAME_DRAW_FRAME_EVENT, CTMDrawFrameEvent> : public TrueValue {};
#pragma endregion



#pragma region IsConcreteEventType
	template <typename ConcreteEventTy>
	struct IsConcreteEventType : public FalseValue {};

	template <>
	struct IsConcreteEventType<CTMStartEvent> : public TrueValue {};

	template <>
	struct IsConcreteEventType<CTMEndEvent> : public TrueValue {};



	template <>
	struct IsConcreteEventType<CTMMouseMoveEvent> : public TrueValue {};


	
	template <>
	struct IsConcreteEventType<CTMClearFrameEvent> : public TrueValue {};

	template <>
	struct IsConcreteEventType<CTMStartFrameEvent> : public TrueValue {};

	template <>
	struct IsConcreteEventType<CTMDrawFrameEvent> : public TrueValue {};
#pragma endregion



#pragma region EnumGenericEventTypeOf
	template <typename GenericEventTy>
	struct EnumGenericEventTypeOf {};

	template <>
	struct EnumGenericEventTypeOf<CTMStartEvent> {
		static constexpr CTMGenericEventType Type = CTMGenericEventType::CTM_STATE_EVENT;
	};

	template <>
	struct EnumGenericEventTypeOf<CTMEndEvent> {
		static constexpr CTMGenericEventType Type = CTMGenericEventType::CTM_STATE_EVENT;
	};



	template <>
	struct EnumGenericEventTypeOf<CTMMouseMoveEvent> {
		static constexpr CTMGenericEventType Type = CTMGenericEventType::CTM_MOUSE_EVENT;
	};



	template <>
	struct EnumGenericEventTypeOf<CTMClearFrameEvent> {
		static constexpr CTMGenericEventType Type = CTMGenericEventType::CTM_FRAME_EVENT;
	};

	template <>
	struct EnumGenericEventTypeOf<CTMStartFrameEvent> {
		static constexpr CTMGenericEventType Type = CTMGenericEventType::CTM_FRAME_EVENT;
	};

	template <>
	struct EnumGenericEventTypeOf<CTMDrawFrameEvent> {
		static constexpr CTMGenericEventType Type = CTMGenericEventType::CTM_FRAME_EVENT;
	};
#pragma endregion



#pragma region EnumConcreteEventTypeOf
	template <typename ConcreteEventTy>
	struct EnumConcreteEventTypeOf {};

	template <>
	struct EnumConcreteEventTypeOf<CTMStartEvent> {
		static constexpr CTMConcreteEventType Type = CTMConcreteEventType::CTM_STATE_START_EVENT;
	};

	template <>
	struct EnumConcreteEventTypeOf<CTMEndEvent> {
		static constexpr CTMConcreteEventType Type = CTMConcreteEventType::CTM_STATE_END_EVENT;
	};



	template <>
	struct EnumConcreteEventTypeOf<CTMMouseMoveEvent> {
		static constexpr CTMConcreteEventType Type = CTMConcreteEventType::CTM_MOUSE_MOVE_EVENT;
	};



	template <>
	struct EnumConcreteEventTypeOf<CTMClearFrameEvent> {
		static constexpr CTMConcreteEventType Type = CTMConcreteEventType::CTM_FRAME_CLEAR_FRAME_EVENT;
	};

	template <>
	struct EnumConcreteEventTypeOf<CTMStartFrameEvent> {
		static constexpr CTMConcreteEventType Type = CTMConcreteEventType::CTM_FRAME_START_FRAME_EVENT;
	};

	template <>
	struct EnumConcreteEventTypeOf<CTMDrawFrameEvent> {
		static constexpr CTMConcreteEventType Type = CTMConcreteEventType::CTM_FRAME_DRAW_FRAME_EVENT;
	};
#pragma endregion



#pragma endregion



#pragma region Template Implementations
	template <typename DerivedTy, CTMConcreteEventType EnumConcreteTy, CTMGenericEventType EnumGenericTy>
	CTMEvent<DerivedTy, EnumConcreteTy, EnumGenericTy>::CTMEvent() noexcept
		: ICTMEvent(EnumConcreteTy, EnumGenericTy)
	{
	}

	template <typename DerivedTy, CTMConcreteEventType EnumConcreteTy, CTMGenericEventType EnumGenericTy>
	constexpr [[nodiscard]] bool CTMEvent<DerivedTy, EnumConcreteTy, EnumGenericTy>::IsInstanceOf(const ICTMEvent* pEvent) noexcept
	{
		RUNTIME_ASSERT(pEvent != nullptr, "The provided IEvent is nullptr.\n");

		return pEvent->ConcreteType() == EnumConcreteTy;
	}

	template <typename DerivedTy, CTMConcreteEventType EnumConcreteTy, CTMGenericEventType EnumGenericTy>
	constexpr [[nodiscard]] DerivedTy* CTMEvent<DerivedTy, EnumConcreteTy, EnumGenericTy>::Cast(ICTMEvent* pEvent) noexcept
	{
		RUNTIME_ASSERT(pEvent != nullptr, "The provided IEvent is nullptr.\n");
		RUNTIME_ASSERT(IsInstanceOf(pEvent), "The event's ConcreteEventType doesn't match.");

		return static_cast<DerivedTy*>(pEvent);
	}

	template <typename DerivedTy, CTMConcreteEventType EnumConcreteTy>
	inline CTMMouseEvent<DerivedTy, EnumConcreteTy>::CTMMouseEvent(unsigned int posX, unsigned int posY) noexcept
		: m_PosX(posX), m_PosY(posY) 
	{
	}
#pragma endregion
}