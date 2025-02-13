#pragma once

#include "Core/CoreMacros.hpp"
#include "DirectX/Window/DXWindowGeometry.hpp"
#include "DirectX/Graphics/DXGraphicsUtility.hpp"

namespace CTMRenderer::CTMDirectX::Window::Graphics
{
	class Quad
	{
	public:
		Quad() = default;
		~Quad() = default;
	public:
		inline Quad(float left, float top, float right, float bottom, float relativeWidth, float relativeHeight) noexcept
		{
			RUNTIME_ASSERT(right > left, "Right X position must be larger than left X position.\n");
			RUNTIME_ASSERT(bottom > left, "Bottom Y position must be larger than top Y position.\n");
			RUNTIME_ASSERT(relativeWidth > 0, "Relative width must be greater than 0.\n");
			RUNTIME_ASSERT(relativeHeight > 0, "Relative height must be greater than 0.\n");

			m_NormLeft = ToClipSpaceX(left, relativeWidth);
			m_NormTop = ToClipSpaceY(top, relativeHeight);
			m_NormRight = ToClipSpaceX(right, relativeWidth);
			m_NormBottom = ToClipSpaceY(bottom, relativeHeight);
		}
	public:
		inline constexpr [[nodiscard]] float Left() const noexcept { return m_NormLeft; }
		inline constexpr [[nodiscard]] float Top() const noexcept { return m_NormTop; }
		inline constexpr [[nodiscard]] float Right() const noexcept { return m_NormRight; }
		inline constexpr [[nodiscard]] float Bottom() const noexcept { return m_NormBottom; }
	private:
		float m_NormLeft = 0, m_NormTop = 0, m_NormRight = 0, m_NormBottom = 0;
	};
}