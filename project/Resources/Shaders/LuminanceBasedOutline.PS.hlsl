#include "CopyImage.hlsli"

// LuminanceBasedOutline.PS
Texture2D<float4> gTexture : register(t0);
SamplerState gSampler : register(s0);

cbuffer LuminanceOutlineParam : register(b1)
{
    float threshold; // エッジ強度のしきい値
    float edgeWidth; // サンプリングのピクセル間隔
    float edgeIntensity; // 輪郭の強調度 (0～1)
    float3 edgeColor; // 輪郭の色
    float padding; // 16バイト境界パディング
};

static const float kPrewittHorizontalKernel[3][3] =
{
    { -1.0f / 6.0f, 0.0f, 1.0f / 6.0f },
    { -1.0f / 6.0f, 0.0f, 1.0f / 6.0f },
    { -1.0f / 6.0f, 0.0f, 1.0f / 6.0f }
};

static const float kPrewittVerticalKernel[3][3] =
{
    { -1.0f / 6.0f, -1.0f / 6.0f, -1.0f / 6.0f },
    { 0.0f, 0.0f, 0.0f },
    { 1.0f / 6.0f, 1.0f / 6.0f, 1.0f / 6.0f }
};

static const float2 kIndex3x3[3][3] =
{
    { { -1.0f, -1.0f }, { 0.0f, -1.0f }, { 1.0f, -1.0f } },
    { { -1.0f, 0.0f }, { 0.0f, 0.0f }, { 1.0f, 0.0f } },
    { { -1.0f, 1.0f }, { 0.0f, 1.0f }, { 1.0f, 1.0f } }
};

struct PixelShaderOutput
{
    float4 color : SV_TARGET0;
};

float Luminance(float3 color)
{
    return dot(color, float3(0.2126, 0.7152, 0.0722)); // 標準の輝度計算
}

PixelShaderOutput main(VertexShaderOutput input)
{
    PixelShaderOutput output;

    // 元の画像カラーを取得
    float4 baseColor = gTexture.Sample(gSampler, input.texcoord);

    // Prewittフィルタの水平・垂直勾配を計算
    float gx = 0.0f;
    float gy = 0.0f;

    for (int y = 0; y < 3; ++y)
    {
        for (int x = 0; x < 3; ++x)
        {
            float2 offset = kIndex3x3[y][x] * edgeWidth * float2(ddx(input.texcoord.x), ddy(input.texcoord.y));
            float lum = Luminance(gTexture.Sample(gSampler, input.texcoord + offset).rgb);

            gx += lum * kPrewittHorizontalKernel[y][x];
            gy += lum * kPrewittVerticalKernel[y][x];
        }
    }

    // エッジ強度
    float edge = sqrt(gx * gx + gy * gy);

    // 輪郭判定：しきい値以上なら輪郭色で塗り替え
    if (edge > threshold)
    {
        output.color = float4(edgeColor * edgeIntensity, 1.0f);
    }
    else
    {
        output.color = baseColor; // 元の画像をそのまま
    }

    return output;
}
