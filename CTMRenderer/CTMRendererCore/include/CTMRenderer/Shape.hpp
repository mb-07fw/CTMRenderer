#pragma once

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
	private:
		const ShapeType m_Type;
	};

	class IRectangle
	{
	public:
		IRectangle(float left, float top, float right, float bottom) noexcept;
		virtual ~IRectangle() = default;
	public:
		inline [[nodiscard]] float Left() const noexcept { return m_Left; }
		inline [[nodiscard]] float Top() const noexcept { return m_Top; }
		inline [[nodiscard]] float Right() const noexcept { return m_Right; }
		inline [[nodiscard]] float Bottom() const noexcept { return m_Bottom; }
	private:
		float m_Left, m_Top, m_Right, m_Bottom;
	};
}