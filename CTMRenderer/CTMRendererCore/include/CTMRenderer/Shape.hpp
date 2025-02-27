#pragma once

#include "CTMRenderer/Color.hpp"

namespace CTMRenderer::Shapes
{
	enum class ShapeType
	{
		RECT,
		CIRClE
	};

	class IShape
	{
	public:
		IShape(ShapeType type) noexcept;
		virtual ~IShape() = default;
	public:
		[[nodiscard]] const char* TypeToStr() const noexcept;
		inline [[nodiscard]] ShapeType Type() const noexcept { return m_Type; }
	protected:
		const ShapeType m_Type;
		bool m_IsValidated = false;
	};

	class IRectangle : public IShape
	{
	public:
		IRectangle(float left, float top, float right, float bottom, Color color) noexcept;
		virtual ~IRectangle() = default;
	public:
		inline [[nodiscard]] float Left() const noexcept { return m_Left; }
		inline [[nodiscard]] float Top() const noexcept { return m_Top; }
		inline [[nodiscard]] float Right() const noexcept { return m_Right; }
		inline [[nodiscard]] float Bottom() const noexcept { return m_Bottom; }
	private:
		float m_Left, m_Top, m_Right, m_Bottom;
		Color m_Color;
	};
}