#include "Core/CorePCH.hpp"
#include "CTMRenderer/Shape.hpp"

namespace CTMRenderer::Shapes
{
	IShape::IShape(ShapeType type) noexcept
		: m_Type(type)
	{
	}

	[[nodiscard]] const char* IShape::TypeToStr() const noexcept
	{
		switch (m_Type)
		{
		case ShapeType::RECT:	return "Rectangle";
		case ShapeType::CIRClE: return "Circle";
		default:				return "UNKNOWN";
		}
	}

	IRectangle::IRectangle(float left, float top, float right, float bottom) noexcept
		: m_Left(left), m_Top(top), m_Right(right), m_Bottom(bottom)
	{

	}
}