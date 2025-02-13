#pragma once

#include "Core/CoreMacros.hpp"
#include "DirectX/Graphics/DXQuad.hpp"

namespace CTMRenderer::CTMDirectX::Window::Graphics
{
	class Rect
	{
	public:
		inline Rect(float left, float top, float right, float bottom) noexcept
			: m_Left(left), m_Top(top), m_Right(right), m_Bottom(bottom),
			m_Width(right - left), m_Height(bottom - top)
		{
			RUNTIME_ASSERT(right > left, "Right X position must be larger than left X position.\n");
			RUNTIME_ASSERT(bottom > left, "Bottom Y position must be larger than top Y position.\n");
		}
	public:
		inline Quad AsQuad(float relativeWidth, float relativeHeight) const noexcept
		{
			return Quad(m_Left, m_Top, m_Right, m_Bottom, relativeWidth, relativeHeight);
		}
	public:
		inline constexpr [[nodiscard]] float Left() const noexcept { return m_Left; }
		inline constexpr [[nodiscard]] float Top() const noexcept { return m_Top; }
		inline constexpr [[nodiscard]] float Right() const noexcept { return m_Right; }
		inline constexpr [[nodiscard]] float Bottom() const noexcept { return m_Bottom; }
		inline constexpr [[nodiscard]] float Width() const noexcept { return m_Width; }
		inline constexpr [[nodiscard]] float Height() const noexcept { return m_Height; }
	private:
		float m_Left = 0, m_Top = 0, m_Right = 0, m_Bottom = 0;
		float m_Width = 0, m_Height = 0;
	};
}