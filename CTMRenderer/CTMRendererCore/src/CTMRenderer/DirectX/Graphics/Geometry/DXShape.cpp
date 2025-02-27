#include "Core/CorePCH.hpp"
#include "CTMRenderer/DirectX/Graphics/Geometry/DXShape.hpp"

namespace CTMRenderer::CTMDirectX::Graphics::Geometry
{
	DXRect::DXRect(float left, float top, float right, float bottom, Shapes::Color color) noexcept
		: IRectangle(left, top, right, bottom, color)
	{
		m_IsValidated = true;
	}
}