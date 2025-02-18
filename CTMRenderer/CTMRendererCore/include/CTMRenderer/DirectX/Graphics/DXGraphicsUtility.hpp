#pragma once

#include "Core/CoreMacros.hpp"

namespace CTMRenderer::CTMDirectX::Graphics
{
    // Converts a screen space x-coordinate to clip space where relativeWidth is the width of the plane, or viewport.
    inline constexpr float ScreenToNDCX(float x, float relativeWidth) noexcept
    {
        RUNTIME_ASSERT(relativeWidth != 0, "A width of 0 will result in division by zero.\n");

        return (2 * x) / relativeWidth - 1;
    }

    // Converts a screen space y-coordinate to clip space where relativeHeight is the height of the plane, or viewport.
    inline constexpr float ScreenToNDCY(float y, float relativeHeight) noexcept
    {
        RUNTIME_ASSERT(relativeHeight != 0, "A height of 0 will result in division by zero.\n");

        return 1 - (2 * y / relativeHeight);
    }

    // Converts a clip space x-coordinate to screen space where relativeWidth is the width of the plane, or viewport.
    inline constexpr float NDCToScreenX(float clipX, float relativeWidth) noexcept
    {
        return (clipX + 1) * relativeWidth * 0.5f;
    }

    // Converts a clip space y-coordinate to screen space where relativeHeight is the height of the plane, or viewport.
    inline constexpr float NDCToScreenY(float clipY, float relativeHeight) noexcept
    {
        return ((clipY + 1) * relativeHeight) * 0.5f;
    }

    // Converts a two dimensional position to clip space.
    inline constexpr void PosToClipSpace(float& outX, float& outY, float x, float y, float relativeWidth, float relativeHeight) noexcept
    {
        outX = ScreenToNDCX(x, relativeWidth);
        outY = ScreenToNDCY(y, relativeHeight);
    }
}