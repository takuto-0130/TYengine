#include "Sprite.hlsli"

cbuffer SpriteTransform : register(b0)
{
    float4x4 matWVP;
};

struct VertexShaderInput
{
    float3 position : POSITION0;
    float2 texCoord : TEXCOORD0;
};

VertexShaderOutput main(VertexShaderInput input)
{
    VertexShaderOutput output;
    output.position = mul(float4(input.position, 1.0f), matWVP);
    output.texCoord = input.texCoord;
    return output;
}