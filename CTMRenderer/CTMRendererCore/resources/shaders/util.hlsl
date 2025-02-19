// Utils.hlsl
#ifndef UTILS_HLSL
#define UTILS_HLSL

float2 ScreenToClipSpace(float2 Pos, float2 WidthHeight)
{
    return float2(
        (2.0f * Pos.x) / WidthHeight.x - 1.0f,
        1.0f - (2.0f * Pos.y) / WidthHeight.y
    );
}

#endif // UTILS_HLSL