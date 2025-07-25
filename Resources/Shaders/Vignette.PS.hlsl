#include "CopyImage.hlsli"

// Vignette.PS
Texture2D<float4> gTexture : register(t0);
SamplerState gSampler : register(s0);

cbuffer VignetteParam : register(b1)
{
    float intensity; // 例: 16.0f
    float power; // 例: 0.8f
    float3 vignetteColor;
};

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
        output.color = float4(0, 0, 0, 0);
        discard;
    }
    
    output.color = gTexture.Sample(gSampler, input.texcoord);
    
    float2 correct = input.texcoord * (1.0f - input.texcoord.yx);
    float vignette = correct.x * correct.y * intensity;
    vignette = saturate(pow(vignette, power));
    
    // 元の色とビネットカラーの線形補間
    output.color.rgb = lerp(vignetteColor, output.color.rgb, vignette);
    return output;
}