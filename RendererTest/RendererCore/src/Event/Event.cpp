#include "Core/CorePCH.hpp"
#include "Event/Event.hpp"

namespace Renderer::Event
{
	[[nodiscard]] std::string Event::AbsTypeToString(AbstractEventType eventType) noexcept
	{
		switch (eventType)
		{
		case AbstractEventType::RENDERER_STATE:	return "RENDERER_STATE";
		//case EventType::MOUSE:			return "MOUSE";
		default:								return "UNKNOWN";
		}
	}

	[[nodiscard]] std::string Event::ConcreteTypeToString(ConcreteEventType concreteType) noexcept
	{
		switch (concreteType)
		{
		case ConcreteEventType::RENDERER_START: return "RENDERER_START";
		case ConcreteEventType::RENDERER_END:	return "RENDERER_END";
		default:								return "UNKNOWN";
		}
	}
	
	RendererStateEvent::RendererStateEvent(ConcreteEventType concreteType, RendererStateEventType stateType) noexcept
		: Event(AbstractEventType::RENDERER_STATE, concreteType), m_StateType(stateType)
	{
	}

	RendererStartEvent::RendererStartEvent(unsigned int placeholderValue) noexcept
		: RendererStateEvent(ConcreteEventType::RENDERER_START, RendererStateEventType::START), m_PlaceholderValue(placeholderValue)
	{
	}

	void RendererStartEvent::Update(unsigned int placeholderValue) noexcept
	{
	}

	RendererEndEvent::RendererEndEvent(float placeholderValue) noexcept
		: RendererStateEvent(ConcreteEventType::RENDERER_START, RendererStateEventType::END), m_PlaceholderValue(placeholderValue)
	{
	}

	void RendererEndEvent::Update(float placeholderValue) noexcept
	{
	}
}