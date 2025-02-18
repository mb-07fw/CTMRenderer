#pragma once

#include "Core/CoreMacros.hpp"
#include "CTMRenderer/DirectX/Window/DXWindowGeometry.hpp"
#include "CTMRenderer/DirectX/Graphics/DXGraphicsUtility.hpp"

namespace CTMRenderer::CTMDirectX::Graphics::Geometry
{
	struct DXQuad;

	struct DXAABB
	{
		inline DXAABB(float left, float top, float right, float bottom)
			: left(left), top(top), right(right), bottom(bottom),
			  width(right - left), height(bottom - top),
			  cX(width / 2), cY(height / 2)
		{
			RUNTIME_ASSERT(right > left, "Right X position must be larger than left X position.\n");
			RUNTIME_ASSERT(bottom > top, "Bottom Y position must be larger than top Y position.\n");
		}

		~DXAABB() = default;

		DXQuad AsQuad(float relativeWidth, float relativeHeight) const noexcept;

		float left, top, right, bottom;
		float width, height;
		float cX, cY;
	};

	struct DXQuad
	{
		inline DXQuad(const DXAABB& aabb, float relativeWidth, float relativeHeight) noexcept
		{
			RUNTIME_ASSERT(relativeWidth > 0, "Relative width must be greater than 0.\n");
			RUNTIME_ASSERT(relativeHeight > 0, "Relative height must be greater than 0.\n");

			NormLeft = ScreenToNDCX(aabb.left, relativeWidth);
			NormTop = ScreenToNDCY(aabb.top, relativeHeight);
			NormRight = ScreenToNDCX(aabb.right, relativeWidth);
			NormBottom = ScreenToNDCY(aabb.bottom, relativeHeight);
		}

		inline DXQuad(float left, float top, float right, float bottom, float relativeWidth, float relativeHeight) noexcept
		{
			RUNTIME_ASSERT(right > left, "Right X position must be larger than left X position.\n");
			RUNTIME_ASSERT(bottom > top, "Bottom Y position must be larger than top Y position.\n");
			RUNTIME_ASSERT(relativeWidth > 0, "Relative width must be greater than 0.\n");
			RUNTIME_ASSERT(relativeHeight > 0, "Relative height must be greater than 0.\n");

			NormLeft = ScreenToNDCX(left, relativeWidth);
			NormTop = ScreenToNDCY(top, relativeHeight);
			NormRight = ScreenToNDCX(right, relativeWidth);
			NormBottom = ScreenToNDCY(bottom, relativeHeight);
		}

		~DXQuad() = default;

		float NormLeft = 0, NormTop = 0, NormRight = 0, NormBottom = 0;
	};
}