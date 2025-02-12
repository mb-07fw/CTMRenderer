#pragma once

#include "Core/CoreMacros.hpp"

namespace CTMRenderer::CTMDirectX::Window::Graphics
{
    // Converts a screen space x-coordinate to clip space where xMin is
    // the left of screen space, and height is the height of the screen.
    inline constexpr float ToClipSpaceX(float x, float xMin, float width)
    {
        RUNTIME_ASSERT(width != 0, "A width of 0 will result in division by zero.\n");

        return (2 * (x - xMin)) / width - 1;
    }

    // Converts a screen space y-coordinate to clip space where yMin is
    // the top of screen space, and height is the height of the screen.
    inline constexpr float ToClipSpaceY(float y, float yMin, float height)
    {
        RUNTIME_ASSERT(height != 0, "A height of 0 will result in division by zero.\n");

        return 1 - (2 * (y - yMin) / height);
    }
}