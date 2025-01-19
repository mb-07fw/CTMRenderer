#pragma once

#include <string>

namespace Renderer::Event
{
	enum class EventType
	{
		INVALID = -1,
		RENDERER_START,
		RENDERER_END
	};

	// TODO: Make Event be an abstract class for further events such as control input. (keyboard and mouse)
	class Event
	{
	public:
		inline Event(EventType type)
			: m_Type(type) {}
	public:
		constexpr inline EventType Type() const { return m_Type; }
	public:
		inline static std::string TypeToString(EventType eventType)
		{
			switch (eventType)
			{
			case EventType::INVALID:		return "INVALID";
			case EventType::RENDERER_START:	return "RENDERER_START";
			default:						return "UNKNOWN EVENT TYPE.";
			}
		}
	private:
		EventType m_Type;
	};
}