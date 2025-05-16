#include "CopyImage.hlsli"

// DepthBasedOutline.PS
Texture2D<float4> gTexture : register(t0);
SamplerState gSampler : register(s0);

Texture2D<float> gDepthTexture : register(t1);

SamplerState gSamplerPoint : register(s1);

cbuffer CopyPassParam : register(b0)
{
    float2 offset;
    float2 scale;
    float4x4 invProjection;
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
    uint width, height;
    gTexture.GetDimensions(width, height);
    float2 uvStepSize = float2(1.0 / width, 1.0 / height);

    float2 difference = float2(0.0f, 0.0f);
    float4 viewPos;
    float ndcDepth;
    for (int x = 0; x < 3; ++x)
    {
        for (int y = 0; y < 3; ++y)
        {
            float2 texcoord = input.texcoord + kIndex3x3[x][y] * uvStepSize;

            ndcDepth = gDepthTexture.Sample(gSamplerPoint, texcoord);
            float4 clipPos = float4(texcoord * 2.0f - 1.0f, ndcDepth, 1.0f);
            viewPos = mul(clipPos, invProjection);
            viewPos /= viewPos.w;

            float luminance = viewPos.z;

            difference.x += luminance * kPrewittHorizontalKernel[x][y];
            difference.y += luminance * kPrewittVerticalKernel[x][y];
        }
    }

    float weight = saturate(length(difference));

    output.color.rgb = ndcDepth.xxx/*(1.0f - weight) * gTexture.Sample(gSampler, input.texcoord).rgb*/;
    output.color.a = 1.0f;

    if (input.texcoord.x < 0.0f || input.texcoord.x > 1.0f ||
        input.texcoord.y < 0.0f || input.texcoord.y > 1.0f)
    {
        discard;
    }
    return output;
}