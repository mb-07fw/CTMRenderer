cbuffer ScreenSpace : register(b0)
{
    float2 WidthHeight;
}

struct VSOutput
{
    float4 Color : PX_COLOR; // Note to self : Keep color first or else.
    float4 Pos : SV_Position;
};

struct VertexInput
{
    float2 Pos : POSITION;
};

struct InstanceInput
{
    float2 ScalarXY : INSTANCE_SCALAR;
    float2 OffsetPos : INSTANCE_OFFSET;
    float4 Color : INSTANCE_COLOR;
};

float2 ScreenToClipSpace(float2 Pos)
{
    return float2(
        (2 * Pos.x) / WidthHeight.x - 1,
        1 - (2 * Pos.y / WidthHeight.y)
    );
}

VSOutput main(VertexInput vInput, InstanceInput iInput)
{
    VSOutput output;
    
    vInput.Pos = vInput.Pos * iInput.ScalarXY;
    
    vInput.Pos += iInput.OffsetPos;
    
    // Convert provided coordinates to screen space.
    output.Pos = float4(ScreenToClipSpace(vInput.Pos), 0.0f, 1.0f);
    
    output.Color = iInput.Color;
    return output;
}