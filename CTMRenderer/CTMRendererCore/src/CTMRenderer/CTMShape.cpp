#include "Core/CorePCH.hpp"
#include "CTMRenderer/CTMShape.hpp"

namespace CTMRenderer::Shapes
{
	CTMShape::CTMShape(CTMShapeType type) noexcept
		: m_Type(type)
	{
	}

	[[nodiscard]] const char* CTMShape::TypeToStr() const noexcept
	{
		switch (m_Type)
		{
		case CTMShapeType::CTM_RECT:	return "CTM_RECT";
		case CTMShapeType::CTM_CIRClE: return "CTM_CIRCLE";
		default:				return "UNKNOWN";
		}
	}

	CTMRect::CTMRect(float left, float top, float right, float bottom, CTMColor color) noexcept
		: CTMShape(CTMShapeType::CTM_RECT),
		  m_Left(left), m_Top(top), 
		  m_Right(right), m_Bottom(bottom),
		  m_Color(color)
	{
	}

	void CTMRect::Set(float left, float top, float right, float bottom, CTMColor color) noexcept
	{
		m_Left = left;
		m_Top = top;
		m_Right = right;
		m_Bottom = bottom;
		m_Color = color;
	}
}