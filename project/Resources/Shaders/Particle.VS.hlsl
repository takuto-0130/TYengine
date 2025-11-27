#include "Particle.hlsli"

StructuredBuffer<ParticleForGPU> gParticles : register(t0);

struct Camera
{
    float3 worldPosition;
};
ConstantBuffer<Camera> gCamera : register(b1);

struct VertexShaderInput
{
    float4 position : POSITION0; // ここは quad のローカル座標(-0.5~0.5)
    float2 texCoord : TEXCOORD0;
    float3 normal : NORMAL0; // 不使用でもOK、残しておく
};

VertexShaderOutput main(VertexShaderInput input, uint instanceId : SV_InstanceID)
{
    VertexShaderOutput output;

    // パーティクルを取得
    ParticleForGPU p = gParticles[instanceId];
    if (p.alive == 0)
    {
        output.position = float4(0, 0, 0, 0); // 完全クリップ
        return output;
    }

    //------------------------------------------------------------------
    // ① カメラ方向のビルボード軸を作る
    //------------------------------------------------------------------
    float3 camPos = gCamera.worldPosition;
    float3 toCamera = normalize(camPos - p.position);

    float3 up = float3(0, 1, 0);
    float3 right = normalize(cross(up, toCamera));
    float3 realUp = cross(toCamera, right);

    //------------------------------------------------------------------
    // ② Quad のローカル頂点をワールド座標へ変換
    //------------------------------------------------------------------
    // input.position.xy が -0.5～0.5 の頂点
    float2 local = input.position.xy;

    // ここで好きなサイズにできる
    float size = 0.1; // パーティクル直径（あなたの環境に合わせて調整）

    float3 worldPos =
        p.position +
        right * (local.x * size) +
        realUp * (local.y * size);

    output.worldPosition = worldPos;

    //------------------------------------------------------------------
    // ③ 最終クリップ座標
    //------------------------------------------------------------------
    float4 clip = mul(float4(worldPos, 1.0), p.WVP);
    output.position = clip;

    //------------------------------------------------------------------
    // ④ その他（元のコードを維持）
    //------------------------------------------------------------------
    output.texCoord = input.texCoord;
    output.color = p.color;

    // normal は疑似的に front-facing にしておく（不要なら削除可能）
    output.normal = toCamera;

    return output;
}
