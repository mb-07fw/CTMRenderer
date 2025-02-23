#include "util.hlsl"

cbuffer Test : register(b0)
{
    float scale;
};

float4 main(float2 pos : POSITION) : SV_POSITION
{
    //pos *= scale;
    
    return float4(pos, 0.0f, 1.0f);
}