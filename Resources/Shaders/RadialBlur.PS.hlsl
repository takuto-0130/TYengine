#include "CopyImage.hlsli"

// RadialBlur.PS
Texture2D<float4> gTexture : register(t0);
SamplerState gSamplerLinear : register(s0);

// ブラー用定数バッファ
//cbuffer BlurSettings : register(b1)
//{
//    float2 kCenter; // ブラーの中心（例: 0.5, 0.5）
//    float kBlurWidth; // ブラーの広がり（例: 0.08）
//    int kNumSamples; // サンプル数（例: 10）
//}
static const float2 kCenter = float2(0.5f, 0.5f);
static const float kBlurWidth = 0.08f;
static const int kNumSamples = 10;

// 出力構造
struct PixelShaderOutput
{
    float4 color : SV_TARGET0;
};

PixelShaderOutput main(VertexShaderOutput input)
{
    PixelShaderOutput output;

    float2 direction = input.texcoord - kCenter;
    float3 outputColor = float3(0.0f, 0.0f, 0.0f);
    for (int sampleIndex = 0; sampleIndex < kNumSamples; ++sampleIndex)
    {
    // 左右のブラーの方向に交互にずらすサンプル位置を計算してテクスチャをサンプリングして加算
        float2 texcoord = input.texcoord - direction * kBlurWidth * float(sampleIndex);
        outputColor.rgb += gTexture.Sample(gSamplerLinear, texcoord).rgb;
    }
    
    

    // 平均化する
    outputColor.rgb *= rcp(float(kNumSamples));
    output.color.rgb = outputColor;
    output.color.a = 1.0f;
    return output;
}