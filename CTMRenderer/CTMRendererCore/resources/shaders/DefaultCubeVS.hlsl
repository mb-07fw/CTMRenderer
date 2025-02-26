cbuffer TranslationBuffer : register(b0)
{
    matrix rotation;
}

struct VSOutput
{
    float4 color : Color; // Note to self : Keep color first. Trust me.
    float4 pos : SV_Position;
};

VSOutput main(float3 pos : Position, float4 color : Color)
{
    VSOutput output;
    
    output.color = color;
    output.pos = float4(pos, 1.0f);
    
    output.pos = mul(output.pos, rotation);
    
    return output; // Convert 2D coordinate to 4D homogenous space.
}