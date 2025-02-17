#include "Core/CorePCH.hpp"
#include "Renderer/DirectX/Graphics/DXAABB.hpp"

namespace CTMRenderer::CTMDirectX::Graphics
{
	DXQuad DXAABB::AsQuad(float relativeWidth, float relativeHeight) const noexcept
	{
		return DXQuad(*this, relativeWidth, relativeHeight);
	}
}