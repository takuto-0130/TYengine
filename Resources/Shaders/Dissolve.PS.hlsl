#include "CopyImage.hlsli"

// Dissolve.PS
Texture2D<float4> gTexture : register(t0);
SamplerState gSampler : register(s0);

Texture2D<float> gMaskTexture : register(t1);

cbuffer DissolveSettings : register(b1)
{
    float threshold;
    bool useEdge;
}

struct PixelShaderOutput
{
    float4 color : SV_TARGET0;
};

PixelShaderOutput main(VertexShaderOutput input)
{
    PixelShaderOutput output;
    
    float mask = gMaskTexture.Sample(gSampler, input.texcoord);
    if (mask < threshold)
    {
        discard;
    }
    
    if (input.texcoord.x < 0.0f || input.texcoord.x > 1.0f ||
    input.texcoord.y < 0.0f || input.texcoord.y > 1.0f)
    {
        discard;
    }
    
    output.color = gTexture.Sample(gSampler, input.texcoord);
    
    // Edge
    if (useEdge)
    {
        float edge = 1.0f - smoothstep(threshold, threshold + 0.05f, mask);
        output.color.rgb += edge * float3(1.0f, 0.4f, 0.3f);
    }
    
    return output;
}