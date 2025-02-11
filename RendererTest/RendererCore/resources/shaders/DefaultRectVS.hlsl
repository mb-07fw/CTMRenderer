struct VSOutput
{
    float4 color : Color; // Note to self : Keep color first or things will go wrong.
    float4 pos : SV_Position;
};

float4 main( float2 pos : Position ) : SV_POSITION
{
    return float4(pos, 0.0f, 1.0f);
}