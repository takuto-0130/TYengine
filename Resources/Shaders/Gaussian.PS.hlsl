#include "CopyImage.hlsli"

// Gaussian.PS
Texture2D<float4> gTexture : register(t0);
SamplerState gSampler : register(s0);

struct PixelShaderOutput
{
    float4 color : SV_TARGET0;
};

static const int kKernelSizeMax = 7;

void generateOffsetTable(
    in int kernelSize, // 3, 5, 7など（奇数）
    out float2 indexOffset[kKernelSizeMax][kKernelSizeMax] // 最大サイズまで確保
)
{
    int halfSize = kernelSize / 2;

    for (int y = -halfSize; y <= halfSize; ++y)
    {
        for (int x = -halfSize; x <= halfSize; ++x)
        {
            indexOffset[y + halfSize][x + halfSize] = float2(float(x), float(y));
        }
    }
}

static const float PI = 3.14159265f;

float gauss(float x, float y, float sigma)
{
    float exponent = -(x * x + y * y) * rcp(2.0f * sigma * sigma);
    float denominator = 2.0f * PI * sigma * sigma;
    return exp(exponent) * rcp(denominator);
}

// カーネル
void generateGaussianKernel(
    in int kernelSize, // 3, 5, 7 とかの奇数
    in float sigma,
    out float kernel[kKernelSizeMax][kKernelSizeMax]
)
{
    int halfSize = kernelSize / 2;
    float sum = 0.0f;

    for (int y = -halfSize; y <= halfSize; ++y)
    {
        for (int x = -halfSize; x <= halfSize; ++x)
        {
            float w = gauss(float(x), float(y), sigma);
            kernel[y + halfSize][x + halfSize] = w;
            sum += w;
        }
    }

    // 正規化（合計が1になるように）
    for (int y = 0; y < kernelSize; ++y)
    {
        for (int x = 0; x < kernelSize; ++x)
        {
            kernel[y][x] /= sum;
        }
    }
}

PixelShaderOutput main(VertexShaderOutput input)
{
    PixelShaderOutput output;
    uint width, height;
    gTexture.GetDimensions(width, height);
    float2 uvStepSize = float2(rcp(float(width)), rcp(float(height)));

    output.color.rgb = float3(0.0f, 0.0f, 0.0f);
    output.color.a = 1.0f;
    
    float kernel[kKernelSizeMax][kKernelSizeMax]; // 最大サイズを確保
    float2 kIndex[kKernelSizeMax][kKernelSizeMax];
    int kernelSize = 5;
    generateOffsetTable(kernelSize, kIndex);
    generateGaussianKernel(kernelSize, 5.0f, kernel);

    for (int y = 0; y < kernelSize; ++y)
    {
        for (int x = 0; x < kernelSize; ++x)
        {
            float2 texcoord = input.texcoord + kIndex[y][x] * uvStepSize;
            float3 fetchColor = gTexture.Sample(gSampler, texcoord).rgb;
            output.color.rgb += fetchColor * kernel[y][x];
        }
    }

    if (input.texcoord.x < 0.0f || input.texcoord.x > 1.0f ||
        input.texcoord.y < 0.0f || input.texcoord.y > 1.0f)
    {
        discard;
    }

    return output;
}
