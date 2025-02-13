#pragma once

#include "DirectX/Window/DXWindowGeometry.hpp"
#include "Core/CoreMacros.hpp"

namespace CTMRenderer::CTMDirectX::Window::Graphics
{
    // Converts a screen space x-coordinate to clip space where xMin is
    // the left of screen space, and relativeWeight is the height of the screen.
    inline constexpr float ToClipSpaceX(float x, float relativeWidth) noexcept
    {
        RUNTIME_ASSERT(relativeWidth != 0, "A width of 0 will result in division by zero.\n");

        return (2 * x) / relativeWidth - 1;
    }

    // Converts a screen space y-coordinate to clip space where yMin is
    // the top of screen space, and relativeHeight is the height of the screen.
    inline constexpr float ToClipSpaceY(float y, float relativeHeight) noexcept
    {
        RUNTIME_ASSERT(relativeHeight != 0, "A height of 0 will result in division by zero.\n");

        return 1 - (2 * y / relativeHeight);
    }

    // Converts a two dimensional position to clip space.
    inline constexpr void PosToClipSpace(float& outX, float& outY, float x, float y, float relativeWidth, float relativeHeight) noexcept
    {
        outX = ToClipSpaceX(x, relativeWidth);
        outY = ToClipSpaceY(y, relativeHeight);
    }
}