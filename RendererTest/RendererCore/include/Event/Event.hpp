#pragma once

#include <string>
#include <type_traits>

namespace Renderer::Event
{
	enum class AbstractEventType
	{
		RENDERER_STATE,
		//MOUSE
	};

	enum class ConcreteEventType
	{
		RENDERER_START,
		RENDERER_END
	};

	class Event
	{
	public:
		inline Event(AbstractEventType absType, ConcreteEventType concreteType)
			: m_AbsType(absType), m_ConcreteType(concreteType) {
		}
		virtual ~Event() = default;
	public:
		inline [[nodiscard]] AbstractEventType AbsType() const noexcept { return m_AbsType; }
		inline [[nodiscard]] ConcreteEventType ConcreteType() const noexcept { return m_ConcreteType; }
		inline static [[nodiscard]] std::string AbsTypeToString(AbstractEventType absType) noexcept;
		inline static [[nodiscard]] std::string ConcreteTypeToString(ConcreteEventType concreteType) noexcept;
	private:
		AbstractEventType m_AbsType;
		ConcreteEventType m_ConcreteType;
	};

#pragma region RendererStateEvent
	enum class RendererStateEventType
	{
		START,
		END
	};

	class RendererStateEvent : public Event
	{
	public:
		RendererStateEvent(ConcreteEventType concreteType, RendererStateEventType stateType) noexcept;
		virtual ~RendererStateEvent() = default;
	public:
		inline [[nodiscard]] RendererStateEventType StateType() const noexcept { return m_StateType; }
	private:
		RendererStateEventType m_StateType;
	};

	class RendererStartEvent : public RendererStateEvent
	{
	public:
		RendererStartEvent(unsigned int placeholderValue) noexcept;
		~RendererStartEvent() = default;
	public:
		void Update(unsigned int placeholderValue) noexcept;
		inline [[nodiscard]] float Value() const noexcept { return m_PlaceholderValue; }
	private:
		unsigned int m_PlaceholderValue;
	};

	class RendererEndEvent : public RendererStateEvent
	{
	public:
		RendererEndEvent(float placeholderValue) noexcept;
		~RendererEndEvent() = default;
	public:
		void Update(float placeholderValue) noexcept;
		inline [[nodiscard]] float Value() const noexcept { return m_PlaceholderValue; }
	private:
		float m_PlaceholderValue;
	};
#pragma endregion

#pragma region Custom Type Traits
	template <typename T, ConcreteEventType ConcreteType>
	struct IsMatchingConcreteEventType : std::false_type {};

	template <>
	struct IsMatchingConcreteEventType<RendererStartEvent, ConcreteEventType::RENDERER_START> : std::true_type {};

	template <>
	struct IsMatchingConcreteEventType<RendererEndEvent, ConcreteEventType::RENDERER_END> : std::true_type {};

	template <typename T, ConcreteEventType ConcreteType>
	struct EventArgsCount {};

	template <>
	struct EventArgsCount<RendererStartEvent, ConcreteEventType::RENDERER_START> {
		static constexpr size_t value = 1; // This is 1 as a placeholder for testing.
	};

	template <>
	struct EventArgsCount<RendererEndEvent, ConcreteEventType::RENDERER_END> {
		static constexpr size_t value = 1; // This is 1 as a placeholder for testing.
	};

	template <typename T, ConcreteEventType ConcreteType>
	struct EventArgsType {};

	template <>
	struct EventArgsType<RendererStartEvent, ConcreteEventType::RENDERER_START> {
		using ArgsType = unsigned int;
	};

	template <>
	struct EventArgsType<RendererEndEvent, ConcreteEventType::RENDERER_END> {
		using ArgsType = float;
	};
	#pragma endregion
}