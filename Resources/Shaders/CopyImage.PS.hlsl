#include "CopyImage.hlsli"

Texture2D<float4> gTexture : register(t0);
SamplerState gSampler : register(s0);

struct PixelShaderOutput
{
    float4 color : SV_TARGET0;
};

PixelShaderOutput main(VertexShaderOutput input)
{
    PixelShaderOutput output;
    
    if (input.texcoord.x < 0.0f || input.texcoord.x > 1.0f ||
    input.texcoord.y < 0.0f || input.texcoord.y > 1.0f)
    {
        discard;
    }
    
    output.color = gTexture.Sample(gSampler, input.texcoord);
    return output;
}
