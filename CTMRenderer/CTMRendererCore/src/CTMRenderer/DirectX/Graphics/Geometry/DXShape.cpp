#include "Core/CorePCH.hpp"
#include "CTMRenderer/DirectX/Graphics/Geometry/DXShape.hpp"

namespace CTMRenderer::CTMDirectX::Graphics::Geometry
{
	IShape::IShape(ShapeType type, Bindable::ImplementedVSType respectiveVSType, Bindable::ImplementedPSType respectivePSType) noexcept
		: m_Type(type), m_VSType(respectiveVSType), m_PSType(respectivePSType)
	{
	}

	DXRect::DXRect(float left, float top, float right, float bottom, DXColor color) noexcept
		: IShape(ShapeType::RECT, Bindable::ImplementedVSType::POS_VS, Bindable::ImplementedPSType::POS_PS), m_AABB(left, top, right, bottom), m_Color(std::move(color))
	{
	}
}