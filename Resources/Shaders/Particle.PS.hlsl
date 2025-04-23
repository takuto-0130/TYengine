#include "Particle.hlsli"

struct Material
{
    float4 color;
    int enableLighting;
    float4x4 uvTransforam;
};

struct Camera
{
    float3 worldPosition;
};

struct DirectionalLight
{
    float4 color;
    float3 direction;
    float intensity;
};

ConstantBuffer<Material> gMaterial : register(b0);
Texture2D<float4> gTexture : register(t1);
SamplerState gSampler : register(s0);
ConstantBuffer<Camera> gCamera : register(b1);

struct PixelShaderOutput
{
    float4 color : SV_TARGET0;
};

PixelShaderOutput main(VertexShaderOutput input)
{
    PixelShaderOutput output;
    float4 transformedUV = mul(float4(input.texCoord, 0.0f, 1.0f), gMaterial.uvTransforam);
    float4 textureColor = gTexture.Sample(gSampler, transformedUV.xy);
    { output.color = gMaterial.color * textureColor * input.color; }
    if(output.color.a == 0.0) {
       discard;
    }
    return output;
}