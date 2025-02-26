#include "Core/CorePCH.hpp"
#include "CTMRenderer/DirectX/Graphics/Geometry/DXShape.hpp"

namespace CTMRenderer::CTMDirectX::Graphics::Geometry
{
	DXShape::DXShape(Shapes::ShapeType type) noexcept
		: Shapes::IShape(type)
	{
	}

	DXRect::DXRect(float left, float top, float right, float bottom, DXColor color) noexcept
		: DXShape(Shapes::ShapeType::RECT), m_AABB(left, top, right, bottom), m_Color(color)
	{
	}
}