#include "CopyImage.hlsli"

// Random.PS
float rand2dTo1d(float2 uv)
{
    return frac(sin(dot(sin(uv), float2(12.9898, 78.233))) * 143758.5453);
}

float hash21(float2 p)
{
    p = frac(p * float2(123.34, 456.21));
    p += dot(p, p + 45.32);
    return frac(p.x * p.y);
}

struct PixelShaderOutput
{
    float4 color : SV_TARGET0;
};

PixelShaderOutput main(VertexShaderOutput input)
{
    PixelShaderOutput output;
    
    float random = rand2dTo1d(input.texcoord * threshold);
    
    output.color = float4(random, random, random, 1.0f);
    
    return output;
}