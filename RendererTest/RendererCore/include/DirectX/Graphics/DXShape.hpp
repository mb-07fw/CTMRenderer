#pragma once

namespace CTMRenderer::CTMDirectX::Window::Graphics
{
	class Rect
	{
	public:
		inline Rect(float left, float top, float right, float bottom) noexcept
			: m_Left(left), m_Top(top), m_Right(right), m_Bottom(bottom)
		{

		}
	public:
		inline [[nodiscard]] float Left() const noexcept { return m_Left; }
		inline [[nodiscard]] float Top() const noexcept { return m_Top; }
		inline [[nodiscard]] float Right() const noexcept { return m_Right; }
		inline [[nodiscard]] float Bottom() const noexcept { return m_Bottom; }
	private:
		float m_Left, m_Top, m_Right, m_Bottom;
		//float m_NormLeft, m_NormTop, m_NormRight, m_NormBottom;
	};
}