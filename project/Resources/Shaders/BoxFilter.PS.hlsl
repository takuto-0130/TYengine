#include "CopyImage.hlsli"

// BoxFilter.PS
Texture2D<float4> gTexture : register(t0);
SamplerState gSampler : register(s0);

cbuffer BoxFilterParam : register(b1)
{
    int kernelSize; // 奇数 (例: 3, 5, 7)
    float2 texelSize;
}

struct PixelShaderOutput
{
    float4 color : SV_TARGET0;
};

PixelShaderOutput main(VertexShaderOutput input)
{
    PixelShaderOutput output;
    float4 sum = float4(0, 0, 0, 0);
    int halfSize = kernelSize / 2;
    int count = 0;

    for (int y = -halfSize; y <= halfSize; ++y)
    {
        for (int x = -halfSize; x <= halfSize; ++x)
        {
            float2 offset = float2(x, y) * texelSize;
            sum += gTexture.Sample(gSampler, input.texcoord + offset);
            count++;
        }
    }

    output.color = sum / count;
    return output;
}
