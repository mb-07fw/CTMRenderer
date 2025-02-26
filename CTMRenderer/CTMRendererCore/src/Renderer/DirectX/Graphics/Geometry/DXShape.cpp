#include "Core/CorePCH.hpp"
#include "CTMRenderer/DirectX/Graphics/Geometry/DXShape.hpp"

namespace CTMRenderer::CTMDirectX::Graphics::Geometry
{
	IShape::IShape(ShapeType type) noexcept
		: m_Type(type)
	{
	}

	DXRect::DXRect(float left, float top, float right, float bottom, DXColor color) noexcept
		: IShape(ShapeType::RECT), m_AABB(left, top, right, bottom), m_Color(color)
	{
	}
}