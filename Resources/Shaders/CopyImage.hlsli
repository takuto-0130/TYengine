struct VertexShaderOutput
{
    float4 position : SV_POSITION;
    float2 texcoord : TEXCOORD0;
};

cbuffer CopyPassParam : register(b0)
{
    float2 offset;
    float2 scale;
};
