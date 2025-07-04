#include "Skybox.hlsli"

struct TransfomationMatrix
{
    float4x4 WVP;
    float4x4 World;
    float4x4 WorldInverseTranspose;
};
ConstantBuffer<TransfomationMatrix> gTransformationMatrix : register(b1);

struct VertexShaderInput
{
    float4 position : POSITION0;
};

VertexShaderOutput main (VertexShaderInput input)
{
    VertexShaderOutput output;
    output.position = mul(input.position, gTransformationMatrix.WVP).xyww;
    output.texcoord = input.position.xyz;
    return output;
}