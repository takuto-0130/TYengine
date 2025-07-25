#include "CopyImage.hlsli"

// DepthBasedOutline.PS
Texture2D<float4> gTexture : register(t0);
SamplerState gSampler : register(s0);

Texture2D<float> gDepth : register(t1);

SamplerState gSamplerPoint : register(s1);


cbuffer OutlineParam : register(b1)
{
    float2 texelSize; // ピクセルサイズ（1.0 / textureSize）
    float depthThreshold; // 境界と判定する深度差のしきい値
    float3 outlineColor;
    float outlineWidth; // 1.0 = 1ピクセル相当
};

struct PixelShaderOutput
{
    float4 color : SV_TARGET0;
};

static const float2 kIndex3x3[3][3] =
{
    { { -1.0f, -1.0f }, { 0.0f, -1.0f }, { 1.0f, -1.0f } },
    { { -1.0f, 0.0f }, { 0.0f, 0.0f }, { 1.0f, 0.0f } },
    { { -1.0f, 1.0f }, { 0.0f, 1.0f }, { 1.0f, 1.0f } },
};

static const float kPrewittHorizontalKernel[3][3] =
{
    { -1.0f / 6.0f, 0.0f, 1.0f / 6.0f },
    { -1.0f / 6.0f, 0.0f, 1.0f / 6.0f },
    { -1.0f / 6.0f, 0.0f, 1.0f / 6.0f },
};

static const float kPrewittVerticalKernel[3][3] =
{
    { -1.0f / 6.0f, -1.0f / 6.0f, -1.0f / 6.0f },
    { 0.0f, 0.0f, 0.0f },
    { 1.0f / 6.0f, 1.0f / 6.0f, 1.0f / 6.0f },
};

float Luminance(float3 v)
{
    return dot(v, float3(0.2125f, 0.7154f, 0.0721f));
};

PixelShaderOutput main(VertexShaderOutput input)
{
    PixelShaderOutput output;

    float centerDepth = gDepth.Sample(gSampler, input.texcoord);
    float edge = 0.0f;

    for (int y = -1; y <= 1; ++y)
    {
        for (int x = -1; x <= 1; ++x)
        {
            float2 offset = float2(x, y) * texelSize * outlineWidth;
            float neighborDepth = gDepth.Sample(gSampler, input.texcoord + offset);
            edge = max(edge, abs(neighborDepth - centerDepth) > depthThreshold ? 1.0f : 0.0f);
        }
    }

    float4 baseColor = gTexture.Sample(gSampler, input.texcoord);
    output.color = lerp(baseColor, float4(outlineColor, 1.0f), edge);

    if (input.texcoord.x < 0.0f || input.texcoord.x > 1.0f ||
        input.texcoord.y < 0.0f || input.texcoord.y > 1.0f)
    {
        discard;
    }
    return output;
}