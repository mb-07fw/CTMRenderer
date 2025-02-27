#pragma once

#include "Core/CoreMacros.hpp"
#include "CTMRenderer/Shape.hpp"
#include "CTMRenderer/DirectX/Graphics/Geometry/DXAABB.hpp"

namespace CTMRenderer::CTMDirectX::Graphics::Geometry
{
	class DXRect : public Shapes::IRectangle
	{
	public:
		DXRect(float left, float top, float right, float bottom, Shapes::Color color) noexcept;
		~DXRect() = default;
	};
}