cbuffer TransformBuffer : register(b0)
{
    float2 translation;
}

struct VSOutput
{
    float4 color : Color;
    float4 pos : SV_Position;
};

VSOutput main(float4 color : Color, float2 pos : Position)
{
    VSOutput output;
    output.color = color;
    
    pos += translation;
    
    output.pos = float4(pos, 0.0f, 1.0f);
    
    return output; // Convert 2D coordinate to 4D homogenous space.
}