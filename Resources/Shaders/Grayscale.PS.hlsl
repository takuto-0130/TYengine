#include "CopyImage.hlsli"
// Grayscale.PS
Texture2D<float4> gTexture : register(t0);
SamplerState gSampler : register(s0);

cbuffer GrayscaleParam : register(b1)
{
    float strength;
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
    
    float4 color = gTexture.Sample(gSampler, input.texcoord);
    float gray = dot(color.rgb, float3(0.2125f, 0.7154f, 0.0721f));
    float3 grayscale = float3(gray, gray, gray);

    // 線形補間：strength に応じて元の色とグレースケールの間をブレンド
    output.color.rgb = lerp(color.rgb, grayscale, strength);
    
    return output;
}
