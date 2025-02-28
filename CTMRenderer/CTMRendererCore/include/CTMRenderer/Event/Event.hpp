#pragma once

#include <string_view>

#include "Core/CoreMacros.hpp"

namespace CTMRenderer::Event
{
	enum class ConcreteEventType
	{
		CTM_STATE_START_EVENT,
		CTM_STATE_END_EVENT,

		CTM_MOUSE_MOVE_EVENT,

		CTM_FRAME_CLEAR_FRAME_EVENT,
		CTM_FRAME_START_FRAME_EVENT,
		CTM_FRAME_DRAW_FRAME_EVENT,
	};



	enum class GenericEventType
	{
		CTM_ANY, // Used to listen for any event type via the dispatcher.
		CTM_STATE_EVENT,
		CTM_MOUSE_EVENT,
		CTM_FRAME_EVENT
	};



	class IEvent
	{
	public:
		IEvent(ConcreteEventType concreteType, GenericEventType genericType) noexcept;
		virtual ~IEvent() = default;
	public:
		[[nodiscard]] std::string_view GenericTypeToStr() const noexcept;
		[[nodiscard]] std::string_view ConcreteTypeToStr() const noexcept;
		
		inline [[nodiscard]] ConcreteEventType ConcreteType() const noexcept { return m_ConcreteType; }
		inline [[nodiscard]] GenericEventType GenericType() const noexcept { return m_GenericType; }
	private:
		ConcreteEventType m_ConcreteType;
		GenericEventType m_GenericType;
	};



	template <typename DerivedTy, ConcreteEventType EnumConcreteTy, GenericEventType EnumGenericTy>
	class Event : public IEvent
	{
	public:
		static constexpr ConcreteEventType EnumConcreteTy = EnumConcreteTy;
		static constexpr GenericEventType EnumGenericTy = EnumGenericTy;
	public:
		Event() noexcept;
		virtual ~Event() = default;
	public:
		static constexpr [[nodiscard]] bool IsInstanceOf(const IEvent* pEvent) noexcept;
		static constexpr [[nodiscard]] DerivedTy* Cast(IEvent* pEvent) noexcept;
	};



#pragma region StateEvent
	template <typename DerivedTy, ConcreteEventType EnumConcreteTy>
	class StateEvent : public Event<DerivedTy, EnumConcreteTy, GenericEventType::CTM_STATE_EVENT>
	{
	public:
		StateEvent() = default;
		virtual ~StateEvent() = default;
	};



	class StartEvent : public StateEvent<StartEvent, ConcreteEventType::CTM_STATE_START_EVENT>
	{
	public:
		StartEvent(unsigned int placeholderArgs) noexcept;
		~StartEvent() = default;
	public:
		void Update(unsigned int newPlaceholderArgs) noexcept;
		inline [[nodiscard]] unsigned int PlaceholderArgs() const noexcept { return m_PlaceholderArgs; }
	private:
		unsigned int m_PlaceholderArgs;
	};



	class EndEvent : public StateEvent<EndEvent, ConcreteEventType::CTM_STATE_END_EVENT>
	{
	public:
		EndEvent(unsigned int placeholderArgs) noexcept;
		~EndEvent() = default;
	public:
		void Update(unsigned int newPlaceholderArgs) noexcept;
		inline [[nodiscard]] unsigned int PlaceholderArgs() const noexcept { return m_PlaceholderArgs; }
	private:
		unsigned int m_PlaceholderArgs;
	};
#pragma endregion



#pragma region Mouse Events
	template <typename DerivedTy, ConcreteEventType EnumConcreteTy>
	class MouseEvent : public Event<DerivedTy, EnumConcreteTy, GenericEventType::CTM_MOUSE_EVENT>
	{
	public:
		MouseEvent(unsigned int posX, unsigned int posY) noexcept;
		virtual ~MouseEvent() = default;
	public:
		inline [[nodiscard]] unsigned int PosX() const noexcept { return m_PosX; }
		inline [[nodiscard]] unsigned int PosY() const noexcept { return m_PosY; }
	protected:
		unsigned int m_PosX;
		unsigned int m_PosY;
	};



	class MouseMoveEvent : public MouseEvent<MouseMoveEvent, ConcreteEventType::CTM_MOUSE_MOVE_EVENT>
	{
	public:
		MouseMoveEvent(unsigned int posX, unsigned int posY) noexcept;
	public:
		void Update(unsigned int newPosX, unsigned int newPosY) noexcept;
	};
#pragma endregion



#pragma region Frame Events
	template <typename DerivedTy, ConcreteEventType EnumConcreteTy>
	class FrameEvent : public Event<DerivedTy, EnumConcreteTy, GenericEventType::CTM_FRAME_EVENT>
	{
	public:
		FrameEvent() = default;
		virtual ~FrameEvent() = default;
	};



	class ClearFrameEvent : public FrameEvent<ClearFrameEvent, ConcreteEventType::CTM_FRAME_CLEAR_FRAME_EVENT>
	{
	public:
		ClearFrameEvent() = default;
		~ClearFrameEvent() = default;

		inline void Update() const noexcept {}
	};



	class StartFrameEvent : public FrameEvent<StartFrameEvent, ConcreteEventType::CTM_FRAME_START_FRAME_EVENT>
	{
	public:
		StartFrameEvent() = default;
		~StartFrameEvent() = default;

		inline void Update() const noexcept {}
	};



	class DrawFrameEvent : public FrameEvent<DrawFrameEvent, ConcreteEventType::CTM_FRAME_DRAW_FRAME_EVENT>
	{
	public:
		DrawFrameEvent() = default;
		~DrawFrameEvent() = default;

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
	template <ConcreteEventType EnumConcreteTy, typename ConcreteEventTy>
	struct IsMatchingConcreteEventType : public FalseValue {};

	template <>
	struct IsMatchingConcreteEventType<ConcreteEventType::CTM_STATE_START_EVENT, StartEvent> : public TrueValue {};

	template <>
	struct IsMatchingConcreteEventType<ConcreteEventType::CTM_STATE_END_EVENT, EndEvent> : public TrueValue {};



	template <>
	struct IsMatchingConcreteEventType<ConcreteEventType::CTM_MOUSE_MOVE_EVENT, MouseMoveEvent> : public TrueValue {};



	template <>
	struct IsMatchingConcreteEventType<ConcreteEventType::CTM_FRAME_CLEAR_FRAME_EVENT, ClearFrameEvent> : public TrueValue {};

	template <>
	struct IsMatchingConcreteEventType<ConcreteEventType::CTM_FRAME_START_FRAME_EVENT, StartFrameEvent> : public TrueValue {};

	template <>
	struct IsMatchingConcreteEventType<ConcreteEventType::CTM_FRAME_DRAW_FRAME_EVENT, DrawFrameEvent> : public TrueValue {};
#pragma endregion



#pragma region IsConcreteEventType
	template <typename ConcreteEventTy>
	struct IsConcreteEventType : public FalseValue {};

	template <>
	struct IsConcreteEventType<StartEvent> : public TrueValue {};

	template <>
	struct IsConcreteEventType<EndEvent> : public TrueValue {};



	template <>
	struct IsConcreteEventType<MouseMoveEvent> : public TrueValue {};


	
	template <>
	struct IsConcreteEventType<ClearFrameEvent> : public TrueValue {};

	template <>
	struct IsConcreteEventType<StartFrameEvent> : public TrueValue {};

	template <>
	struct IsConcreteEventType<DrawFrameEvent> : public TrueValue {};
#pragma endregion



#pragma region EnumGenericEventTypeOf
	template <typename GenericEventTy>
	struct EnumGenericEventTypeOf {};

	template <>
	struct EnumGenericEventTypeOf<StartEvent> {
		static constexpr GenericEventType Type = GenericEventType::CTM_STATE_EVENT;
	};

	template <>
	struct EnumGenericEventTypeOf<EndEvent> {
		static constexpr GenericEventType Type = GenericEventType::CTM_STATE_EVENT;
	};



	template <>
	struct EnumGenericEventTypeOf<MouseMoveEvent> {
		static constexpr GenericEventType Type = GenericEventType::CTM_MOUSE_EVENT;
	};



	template <>
	struct EnumGenericEventTypeOf<ClearFrameEvent> {
		static constexpr GenericEventType Type = GenericEventType::CTM_FRAME_EVENT;
	};

	template <>
	struct EnumGenericEventTypeOf<StartFrameEvent> {
		static constexpr GenericEventType Type = GenericEventType::CTM_FRAME_EVENT;
	};

	template <>
	struct EnumGenericEventTypeOf<DrawFrameEvent> {
		static constexpr GenericEventType Type = GenericEventType::CTM_FRAME_EVENT;
	};
#pragma endregion



#pragma region EnumConcreteEventTypeOf
	template <typename ConcreteEventTy>
	struct EnumConcreteEventTypeOf {};

	template <>
	struct EnumConcreteEventTypeOf<StartEvent> {
		static constexpr ConcreteEventType Type = ConcreteEventType::CTM_STATE_START_EVENT;
	};

	template <>
	struct EnumConcreteEventTypeOf<EndEvent> {
		static constexpr ConcreteEventType Type = ConcreteEventType::CTM_STATE_END_EVENT;
	};



	template <>
	struct EnumConcreteEventTypeOf<MouseMoveEvent> {
		static constexpr ConcreteEventType Type = ConcreteEventType::CTM_MOUSE_MOVE_EVENT;
	};



	template <>
	struct EnumConcreteEventTypeOf<ClearFrameEvent> {
		static constexpr ConcreteEventType Type = ConcreteEventType::CTM_FRAME_CLEAR_FRAME_EVENT;
	};

	template <>
	struct EnumConcreteEventTypeOf<StartFrameEvent> {
		static constexpr ConcreteEventType Type = ConcreteEventType::CTM_FRAME_START_FRAME_EVENT;
	};

	template <>
	struct EnumConcreteEventTypeOf<DrawFrameEvent> {
		static constexpr ConcreteEventType Type = ConcreteEventType::CTM_FRAME_DRAW_FRAME_EVENT;
	};
#pragma endregion



#pragma endregion



#pragma region Template Implementations
	template <typename DerivedTy, ConcreteEventType EnumConcreteTy, GenericEventType EnumGenericTy>
	Event<DerivedTy, EnumConcreteTy, EnumGenericTy>::Event() noexcept
		: IEvent(EnumConcreteTy, EnumGenericTy)
	{
	}



	template <typename DerivedTy, ConcreteEventType EnumConcreteTy, GenericEventType EnumGenericTy>
	constexpr [[nodiscard]] bool Event<DerivedTy, EnumConcreteTy, EnumGenericTy>::IsInstanceOf(const IEvent* pEvent) noexcept
	{
		RUNTIME_ASSERT(pEvent != nullptr, "The provided IEvent is nullptr.\n");

		return pEvent->ConcreteType() == EnumConcreteTy;
	}


	
	template <typename DerivedTy, ConcreteEventType EnumConcreteTy, GenericEventType EnumGenericTy>
	constexpr [[nodiscard]] DerivedTy* Event<DerivedTy, EnumConcreteTy, EnumGenericTy>::Cast(IEvent* pEvent) noexcept
	{
		RUNTIME_ASSERT(pEvent != nullptr, "The provided IEvent is nullptr.\n");
		RUNTIME_ASSERT(IsInstanceOf(pEvent), "The event's ConcreteEventType doesn't match.");

		return static_cast<DerivedTy*>(pEvent);
	}



	template <typename DerivedTy, ConcreteEventType EnumConcreteTy>
	inline MouseEvent<DerivedTy, EnumConcreteTy>::MouseEvent(unsigned int posX, unsigned int posY) noexcept
		: m_PosX(posX), m_PosY(posY) 
	{
	}
#pragma endregion
}