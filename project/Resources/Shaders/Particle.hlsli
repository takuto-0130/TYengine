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

    float3 position;
    float pad0;

    float3 velocity;
    float pad1;

    float currentTime;
    float lifeTime;
    uint alive;
    float pad2;
};
