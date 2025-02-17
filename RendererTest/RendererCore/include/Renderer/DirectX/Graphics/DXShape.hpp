#pragma once

#include "Core/CoreMacros.hpp"
#include "Renderer/DirectX/Graphics/DXAABB.hpp"
#include "Renderer/DirectX/Graphics/DXColor.hpp"

namespace CTMRenderer::CTMDirectX::Graphics
{
	struct DXRect
	{
		inline DXRect(float left, float top, float right, float bottom, const DXColor& color) noexcept
			: AABB(left, top, right, bottom), color(color)
		{
		}

		~DXRect() = default;

		DXAABB AABB;
		DXColor color;
	};
}