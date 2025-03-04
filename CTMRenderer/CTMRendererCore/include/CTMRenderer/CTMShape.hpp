#pragma once

#include "CTMRenderer/CTMColor.hpp"

namespace CTMRenderer::Shapes
{
	enum class CTMShapeType
	{
		CTM_RECT,
		CTM_CIRClE
	};



	class CTMShape
	{
	public:
		CTMShape(CTMShapeType type) noexcept;
		~CTMShape() = default;
	public:
		[[nodiscard]] const char* TypeToStr() const noexcept;
		inline [[nodiscard]] CTMShapeType Type() const noexcept { return m_Type; }
	protected:
		const CTMShapeType m_Type;
	};



	class CTMRect : public CTMShape
	{
	public:
		CTMRect(float left, float top, float right, float bottom, CTMColor color) noexcept;
		~CTMRect() = default;
	public:
		void Set(float left, float top, float right, float bottom, CTMColor color) noexcept;
	public:
		inline [[nodiscard]] float Left() const noexcept { return m_Left; }
		inline [[nodiscard]] float Top() const noexcept { return m_Top; }
		inline [[nodiscard]] float Right() const noexcept { return m_Right; }
		inline [[nodiscard]] float Bottom() const noexcept { return m_Bottom; }
	private:
		float m_Left, m_Top, m_Right, m_Bottom;
		CTMColor m_Color;
	};
}