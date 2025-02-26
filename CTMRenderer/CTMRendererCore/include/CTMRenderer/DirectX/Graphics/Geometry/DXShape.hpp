#pragma once

#include "Core/CoreMacros.hpp"
#include "CTMRenderer/Shape.hpp"
#include "CTMRenderer/DirectX/Graphics/Geometry/DXAABB.hpp"
#include "CTMRenderer/DirectX/Graphics/DXColor.hpp"

namespace CTMRenderer::CTMDirectX::Graphics::Geometry
{
	class DXShape : public Shapes::IShape
	{
	public:
		DXShape(Shapes::ShapeType type) noexcept;
		virtual ~DXShape() = default;
	};

	class DXRect : public DXShape
	{
	public:
		DXRect(float left, float top, float right, float bottom, DXColor color) noexcept;
		~DXRect() = default;
	public:
		inline [[nodiscard]] DXAABB& AABB() noexcept { return m_AABB; }
		inline [[nodiscard]] DXColor Color() noexcept { return m_Color; }
	private:
		DXAABB m_AABB;
		DXColor m_Color;
	};
}