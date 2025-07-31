struct VertexShaderOutput
{
    float4 position : SV_POSITION;
    float2 texCoord : TEXCOORD0;
    float3 normal : NORMAL0;
    float3 worldPosition : POSITION1;
    float4 color : COLOR0;
};

struct ParticleForGPU
{
    float4x4 WVP;
    float4x4 World;
    float4 color;

    // シミュレーション用に追加
    float3 position;
    float3 velocity;
    float currentTime;
    float lifeTime;
    uint alive; // 0=dead, 1=alive
};