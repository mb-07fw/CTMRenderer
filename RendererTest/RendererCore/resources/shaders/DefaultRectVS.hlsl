struct VSOutput
{
    float4 color : Color; // Note to self : Keep color first or things will go wrong.
    float4 pos : SV_Position;
};

VSOutput main( float4 color : Color, float2 pos : Position )
{
    VSOutput output;
    output.pos = float4(pos, 0.0f, 1.0f);
    output.color = color;
    return output;
}