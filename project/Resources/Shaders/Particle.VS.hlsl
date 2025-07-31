#include "Particle.hlsli"

StructuredBuffer<ParticleForGPU> gParticles : register(t0);

struct Camera
{
    float3 worldPosition;
};
ConstantBuffer<Camera> gCamera : register(b1);

struct VertexShaderInput
{
    float4 position : POSITION0;
    float2 texCoord : TEXCOORD0;
    float3 normal : NORMAL0;
};

VertexShaderOutput main(VertexShaderInput input, uint instanceId : SV_InstanceID)
{
    VertexShaderOutput output;

    // パーティクル情報を取得
    ParticleForGPU p = gParticles[instanceId];
    
    if (p.alive == 0)
    {
        output.position = float4(0, 0, 0, 0); // 完全クリップ
        return output;
    }

    // WVPを適用
    output.position = mul(input.position, p.WVP);

    // World変換後の座標
    float4 worldPos = mul(input.position, p.World);
    output.worldPosition = worldPos.xyz;

    // 法線をWorld行列の回転成分で変換
    // transpose(inverse(World))を取るのが正しいが、スケールをほぼ使わないので簡易化
    float3x3 world3x3 = (float3x3) p.World;
    output.normal = normalize(mul(input.normal, world3x3));

    // その他
    output.texCoord = input.texCoord;
    output.color = p.color;

    return output;
}