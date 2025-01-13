#pragma once

namespace Renderer::Event
{
	enum class EventType
	{
		INVALID = -1,
		RENDERER_START
	};

	struct Event
	{
	public:
		inline Event(EventType type)
			: type(type) {}
	public:
		EventType type;
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
	};
}