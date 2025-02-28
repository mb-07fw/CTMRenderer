#include "Core/CorePCH.hpp"
#include "CTMRenderer/Event/Event.hpp"

namespace CTMRenderer::Event
{
	IEvent::IEvent(ConcreteEventType concreteType, GenericEventType genericType) noexcept
		: m_ConcreteType(concreteType), m_GenericType(genericType)
	{

	}

	[[nodiscard]] std::string_view IEvent::GenericTypeToStr() const noexcept
	{
		switch (m_GenericType)
		{
		case GenericEventType::CTM_ANY:				RUNTIME_ASSERT(false, "No events should have a generic type of CTM_ANY, as it is a marker type for listeners.\n");
			return std::string_view("CTM_ANY");

		case GenericEventType::CTM_STATE_EVENT:		return std::string_view("CTM_STATE_EVEMT");
		case GenericEventType::CTM_MOUSE_EVENT:		return std::string_view("CTM_MOUSE_EVENT");
		default:									return std::string_view("UNKNOWN");
		}
	}

	[[nodiscard]] std::string_view IEvent::ConcreteTypeToStr() const noexcept
	{
		switch (m_ConcreteType)
		{
		case ConcreteEventType::CTM_MOUSE_MOVE_EVENT:	return std::string_view("MOUSE_MOVE_EVENT");
		default:										return std::string_view("UNKNOWN");
		}
	}

	StartEvent::StartEvent(unsigned int placeholderArgs) noexcept
		: m_PlaceholderArgs(placeholderArgs)
	{
	}

	void StartEvent::Update(unsigned int newPlaceholderArgs) noexcept
	{
		m_PlaceholderArgs = newPlaceholderArgs;
	}

	EndEvent::EndEvent(unsigned int placeholderArgs) noexcept
		: m_PlaceholderArgs(placeholderArgs)
	{
	}

	void EndEvent::Update(unsigned int newPlaceholderArgs) noexcept
	{
		m_PlaceholderArgs = newPlaceholderArgs;
	}

	MouseMoveEvent::MouseMoveEvent(unsigned int posX, unsigned int posY) noexcept
		: MouseEvent(posX, posY)
	{
	}

	void MouseMoveEvent::Update(unsigned int newPosX, unsigned int newPosY) noexcept
	{
		m_PosX = newPosX;
		m_PosY = newPosY;
	}
}