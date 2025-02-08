#pragma once

#include <string_view>

#include "Core/CoreMacros.hpp"

namespace CTMRenderer::Event
{
	enum class ConcreteEventType
	{
		CTM_STATE_START_EVENT,
		CTM_STATE_END_EVENT,
		CTM_MOUSE_MOVE_EVENT
	};

	enum class GenericEventType
	{
		CTM_ANY, // Used to listen for any event type via the dispatcher.
		CTM_STATE_EVENT,
		CTM_MOUSE_EVENT,
	};

	class IEvent
	{
	public:
		virtual ~IEvent() = default;
		virtual constexpr [[nodiscard]] ConcreteEventType ConcreteType() const noexcept = 0;
		virtual constexpr [[nodiscard]] GenericEventType GenericType() const noexcept = 0;

		inline static constexpr [[nodiscard]] std::string_view GenericTypeStr(GenericEventType type) noexcept
		{
			switch (type)
			{
			case GenericEventType::CTM_ANY:				RUNTIME_ASSERT(false, "No events should have a generic type of CTM_ANY, as it is a marker type for listeners.\n");
														return std::string_view("CTM_ANY");
			
			case GenericEventType::CTM_STATE_EVENT:		return std::string_view("CTM_STATE_EVEMT");
			case GenericEventType::CTM_MOUSE_EVENT:		return std::string_view("CTM_MOUSE_EVENT");
			default:									return std::string_view("UNKNOWN");
			}
		}

		inline static constexpr [[nodiscard]] std::string_view ConcreteTypeStr(ConcreteEventType type) noexcept
		{
			switch (type)
			{
			case ConcreteEventType::CTM_MOUSE_MOVE_EVENT:	return std::string_view("MOUSE_MOVE_EVENT");
			default:										return std::string_view("UNKNOWN");
			}
		}
	};

	template <typename Derived, ConcreteEventType EnumConcreteTy, GenericEventType EnumGenericTy>
	class Event : public IEvent
	{
	public:
		static constexpr ConcreteEventType EnumConcreteTy = EnumConcreteTy;
	public:
		virtual ~Event() = default;
	public:
		inline virtual constexpr [[nodiscard]] ConcreteEventType ConcreteType() const noexcept override { return EnumConcreteTy; }
		inline virtual constexpr [[nodiscard]] GenericEventType GenericType() const noexcept override { return EnumGenericTy; }
		inline constexpr [[nodiscard]] std::string_view ConcreteTypeToStr() const noexcept { return ConcreteTypeStr(EnumConcreteTy); }
		inline constexpr [[nodiscard]] std::string_view GenericTypeToStr() const noexcept { return GenericTypeStr(EnumGenericTy); }

		inline static constexpr [[nodiscard]] bool IsInstance(const IEvent* pEvent) noexcept
		{
			RUNTIME_ASSERT(pEvent != nullptr, "The provided IEvent is nullptr.\n");

			return pEvent->ConcreteType() == EnumConcreteTy;
		}

		inline static constexpr [[nodiscard]] Derived* Cast(IEvent* pEvent) noexcept
		{
			RUNTIME_ASSERT(pEvent != nullptr, "The provided IEvent is nullptr.\n");
			RUNTIME_ASSERT(IsInstance(pEvent), "The event's ConcreteEventType doesn't match.");

			return static_cast<Derived*>(pEvent);
		}
	};

#pragma region StateEvent
	template <typename Derived, ConcreteEventType EnumConcreteTy>
	class StateEvent : public Event<Derived, EnumConcreteTy, GenericEventType::CTM_STATE_EVENT>
	{
	public:
		StateEvent() = default;
		virtual ~StateEvent() = default;
	};

	class StartEvent : public StateEvent<StartEvent, ConcreteEventType::CTM_STATE_START_EVENT>
	{
	public:
		inline StartEvent(unsigned int placeholderArgs)
			: m_PlaceholderArgs(placeholderArgs) {}

		~StartEvent() = default;
	public:
		inline void Update(unsigned int newPlaceholderArgs) noexcept { m_PlaceholderArgs = newPlaceholderArgs; }
		inline [[nodiscard]] unsigned int PlaceholderArgs() const noexcept { return m_PlaceholderArgs; }
	private:
		unsigned int m_PlaceholderArgs;
	};

	class EndEvent : public StateEvent<EndEvent, ConcreteEventType::CTM_STATE_END_EVENT>
	{
	public:
		inline EndEvent(unsigned int placeholderArgs)
			: m_PlaceholderArgs(placeholderArgs) {
		}

		~EndEvent() = default;
	public:
		inline void Update(unsigned int newPlaceholderArgs) noexcept { m_PlaceholderArgs = newPlaceholderArgs; }
		inline [[nodiscard]] unsigned int PlaceholderArgs() const noexcept { return m_PlaceholderArgs; }
	private:
		unsigned int m_PlaceholderArgs;
	};
#pragma endregion

	template <typename Derived, ConcreteEventType EnumConcreteTy>
	class MouseEvent : public Event<Derived, EnumConcreteTy, GenericEventType::CTM_MOUSE_EVENT>
	{
	public:
		inline MouseEvent(unsigned int posX, unsigned int posY) noexcept
			: m_PosX(posX), m_PosY(posY) {}

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
		inline MouseMoveEvent(unsigned int posX, unsigned int posY) noexcept
			: MouseEvent(posX, posY) {
		}
	public:
		inline void Update(unsigned int newPosX, unsigned int newPosY) noexcept
		{
			m_PosX = newPosX;
			m_PosY = newPosY;
		}
	};
#pragma region Traits
	// Holds a false bool value.
	struct FalseValue {
		static constexpr bool Value = false;
	};

	// Holds a true bool value.
	struct TrueValue {
		static constexpr bool Value = true;
	};

	template <ConcreteEventType EnumConcreteTy, typename ConcreteEventTy>
	struct IsMatchingConcreteEventType : public FalseValue {};

	template <>
	struct IsMatchingConcreteEventType<ConcreteEventType::CTM_STATE_START_EVENT, StartEvent> : public TrueValue {};

	template <>
	struct IsMatchingConcreteEventType<ConcreteEventType::CTM_STATE_END_EVENT, EndEvent> : public TrueValue {};

	template <>
	struct IsMatchingConcreteEventType<ConcreteEventType::CTM_MOUSE_MOVE_EVENT, MouseMoveEvent> : public TrueValue {};

	template <typename ConcreteEventTy>
	struct IsConcreteEventType : public FalseValue {};

	template <>
	struct IsConcreteEventType<StartEvent> : public TrueValue {};

	template <>
	struct IsConcreteEventType<EndEvent> : public TrueValue {};

	template <>
	struct IsConcreteEventType<MouseMoveEvent> : public TrueValue {};

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
#pragma endregion
}