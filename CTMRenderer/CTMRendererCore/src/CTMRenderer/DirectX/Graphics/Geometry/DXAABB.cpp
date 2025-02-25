#include "Core/CorePCH.hpp"
#include "CTMRenderer/DirectX/Graphics/Geometry/DXAABB.hpp"

namespace CTMRenderer::CTMDirectX::Graphics::Geometry
{
	DXQuad DXAABB::AsQuad(float relativeWidth, float relativeHeight) const noexcept
	{
		return DXQuad(*this, relativeWidth, relativeHeight);
	}
}