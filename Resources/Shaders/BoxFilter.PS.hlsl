#include "CopyImage.hlsli"

// BoxFilter.PS
Texture2D<float4> gTexture : register(t0);
SamplerState gSampler : register(s0);

struct PixelShaderOutput
{
    float4 color : SV_TARGET0;
};

static const float2 kIndex5x5[5][5] =
{
    { { -2.0f, -2.0f }, { -1.0f, -2.0f }, { 0.0f, -2.0f }, { 1.0f, -2.0f }, { 2.0f, -2.0f } },
    { { -2.0f, -1.0f }, { -1.0f, -1.0f }, { 0.0f, -1.0f }, { 1.0f, -1.0f }, { 2.0f, -1.0f } },
    { { -2.0f, 0.0f }, { -1.0f, 0.0f }, { 0.0f, 0.0f }, { 1.0f, 0.0f }, { 2.0f, 0.0f } },
    { { -2.0f, 1.0f }, { -1.0f, 1.0f }, { 0.0f, 1.0f }, { 1.0f, 1.0f }, { 2.0f, 1.0f } },
    { { -2.0f, 2.0f }, { -1.0f, 2.0f }, { 0.0f, 2.0f }, { 1.0f, 2.0f }, { 2.0f, 2.0f } },
};

static const float kKernel5x5[5][5] =
{
    { 1.0f / 25, 1.0f / 25, 1.0f / 25, 1.0f / 25, 1.0f / 25 },
    { 1.0f / 25, 1.0f / 25, 1.0f / 25, 1.0f / 25, 1.0f / 25 },
    { 1.0f / 25, 1.0f / 25, 1.0f / 25, 1.0f / 25, 1.0f / 25 },
    { 1.0f / 25, 1.0f / 25, 1.0f / 25, 1.0f / 25, 1.0f / 25 },
    { 1.0f / 25, 1.0f / 25, 1.0f / 25, 1.0f / 25, 1.0f / 25 },
};

PixelShaderOutput main(VertexShaderOutput input)
{
    PixelShaderOutput output;
    uint width, height;
    gTexture.GetDimensions(width, height);
    float2 uvStepSize = float2(rcp(float(width)), rcp(float(height)));

    output.color.rgb = float3(0.0f, 0.0f, 0.0f);
    output.color.a = 1.0f;

    for (int y = 0; y < 5; ++y)
    {
        for (int x = 0; x < 5; ++x)
        {
            float2 texcoord = input.texcoord + kIndex5x5[y][x] * uvStepSize;
            float3 fetchColor = gTexture.Sample(gSampler, texcoord).rgb;
            output.color.rgb += fetchColor * kKernel5x5[y][x];
        }
    }

    if (input.texcoord.x < 0.0f || input.texcoord.x > 1.0f ||
        input.texcoord.y < 0.0f || input.texcoord.y > 1.0f)
    {
        discard;
    }

    return output;
}
