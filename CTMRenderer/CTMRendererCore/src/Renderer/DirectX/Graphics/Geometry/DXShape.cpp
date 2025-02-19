#include "Core/CorePCH.hpp"
#include "CTMRenderer/DirectX/Graphics/Geometry/DXShape.hpp"

namespace CTMRenderer::CTMDirectX::Graphics::Geometry
{
	IShape::IShape(ShapeType type)
		: m_Type(type)
	{

	}

	DXRect::DXRect(float left, float top, float right, float bottom, const DXColor& color) noexcept
		: IShape(ShapeType::RECT), AABB(left, top, right, bottom), color(color)
	{
	}
}