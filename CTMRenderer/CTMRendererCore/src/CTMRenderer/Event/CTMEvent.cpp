#include "Core/CorePCH.hpp"
#include "CTMRenderer/Event/CTMEvent.hpp"

namespace CTMRenderer::Event
{
	ICTMEvent::ICTMEvent(CTMConcreteEventType concreteType, CTMGenericEventType genericType) noexcept
		: m_ConcreteType(concreteType), m_GenericType(genericType)
	{

	}

	[[nodiscard]] std::string_view ICTMEvent::GenericTypeToStr() const noexcept
	{
		switch (m_GenericType)
		{
		case CTMGenericEventType::CTM_ANY:
			RUNTIME_ASSERT(false, "No events should have a generic type of CTM_ANY, as it is a marker type for listeners.\n");
			return std::string_view("CTM_ANY");
		case CTMGenericEventType::CTM_STATE_EVENT:		return std::string_view("CTM_STATE_EVEMT");
		case CTMGenericEventType::CTM_MOUSE_EVENT:		return std::string_view("CTM_MOUSE_EVENT");
		default:										return std::string_view("UNKNOWN");
		}
	}

	[[nodiscard]] std::string_view ICTMEvent::ConcreteTypeToStr() const noexcept
	{
		switch (m_ConcreteType)
		{
		case CTMConcreteEventType::CTM_MOUSE_MOVE_EVENT:	return std::string_view("MOUSE_MOVE_EVENT");
		default:											return std::string_view("UNKNOWN");
		}
	}

	CTMStartEvent::CTMStartEvent(unsigned int placeholderArgs) noexcept
		: m_PlaceholderArgs(placeholderArgs)
	{
	}

	void CTMStartEvent::Update(unsigned int newPlaceholderArgs) noexcept
	{
		m_PlaceholderArgs = newPlaceholderArgs;
	}

	CTMEndEvent::CTMEndEvent(unsigned int placeholderArgs) noexcept
		: m_PlaceholderArgs(placeholderArgs)
	{
	}

	void CTMEndEvent::Update(unsigned int newPlaceholderArgs) noexcept
	{
		m_PlaceholderArgs = newPlaceholderArgs;
	}

	CTMMouseMoveEvent::CTMMouseMoveEvent(unsigned int posX, unsigned int posY) noexcept
		: CTMMouseEvent(posX, posY)
	{
	}

	void CTMMouseMoveEvent::Update(unsigned int newPosX, unsigned int newPosY) noexcept
	{
		m_PosX = newPosX;
		m_PosY = newPosY;
	}
}