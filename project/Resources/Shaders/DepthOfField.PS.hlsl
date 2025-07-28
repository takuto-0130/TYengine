#include "CopyImage.hlsli"

Texture2D<float4> gSceneTex : register(t0);
SamplerState gSampler : register(s0);
Texture2D<float> gDepthTex : register(t1); // DepthStencil SRV

cbuffer DoFPassParam : register(b1)
{
    float nearZ;
    float farZ;
    float focusDepth;
    float blurScale;
};

float LinearizeDepth(float depth)
{
    // DepthStencilのZ (0~1) を線形化
    return (nearZ * farZ) / (farZ - depth * (farZ - nearZ));
}

float4 main(VertexShaderOutput input) : SV_TARGET
{
    float depth = gDepthTex.Sample(gSampler, input.texcoord);
    float linearDepth = LinearizeDepth(depth);

    // フォーカス距離との差からブラー量を決定
    float coc = abs(linearDepth - focusDepth);
    float radius = saturate(coc / focusDepth) * blurScale;

    // 簡易ブラー
    float2 texel = float2(1.0 / 1280.0, 1.0 / 720.0); // 要定数化
    float4 color = float4(0, 0, 0, 0);
    float weight = 0.0;

    for (int x = -2; x <= 2; x++)
    {
        for (int y = -2; y <= 2; y++)
        {
            float2 offset = texel * float2(x, y) * radius;
            color += gSceneTex.Sample(gSampler, input.texcoord + offset);
            weight += 1.0;
        }
    }
    return color / weight;
}