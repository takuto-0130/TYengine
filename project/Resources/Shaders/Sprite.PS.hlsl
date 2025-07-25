#include "Sprite.hlsli"

Texture2D gTexture : register(t0);
SamplerState gSampler : register(s0);

cbuffer SpriteMaterial : register(b0)
{
    float4 color;
    float4x4 uvTransform;
};

struct PixelShaderOutput
{
    float4 color : SV_TARGET0;
};

PixelShaderOutput main(VertexShaderOutput input) // ✅ これが正しい
{
    float4 transformedUV = mul(float4(input.texCoord, 0.0f, 1.0f), uvTransform);
    float4 textureColor = gTexture.Sample(gSampler, transformedUV.xy);
    PixelShaderOutput output;
    output.color = color * textureColor;
    return output;
}