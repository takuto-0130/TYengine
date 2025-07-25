#include "CopyImage.hlsli"

// Random.PS
Texture2D<float4> gTexture : register(t0);
SamplerState gSampler : register(s0);

cbuffer RandomParam : register(b1)
{
    float t; // ノイズのシード
    float noiseStrength; // ノイズ強度 (0.0～1.0)
    float3 noiseTint; // ノイズの色付け (例: float3(1,1,1) = 白)
};

float rand2dTo1d(float2 uv)
{
    return frac(sin(dot(sin(uv), float2(12.9898, 78.233))) * 143758.5453);
}

struct PixelShaderOutput
{
    float4 color : SV_TARGET0;
};

PixelShaderOutput main(VertexShaderOutput input)
{
    PixelShaderOutput output;

    // 元画像カラー
    float4 baseColor = gTexture.Sample(gSampler, input.texcoord);

    // ノイズ値生成 (0.0～1.0)
    float random = rand2dTo1d(input.texcoord * (t + 0.2f));

    // ノイズカラー + 色付け
    float3 noiseColor = lerp(float3(1.0, 1.0, 1.0), noiseTint * random, noiseStrength);

    // 乗算で暗部を作る
    output.color = float4(baseColor.rgb * noiseColor, baseColor.a);

    return output;
}