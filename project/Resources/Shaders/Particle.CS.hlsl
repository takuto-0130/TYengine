#include "Particle.hlsli"

struct EmitterState
{
    uint emitRemaining; // 残り発生数
    uint emitThisFrame; // このフレーム発生予定数
    float emitTimer; // SEQUENTIAL用タイマー
    uint emitMode; // 1=LOOP, 2=ONESHOT, 3=SEQUENTIAL
};

RWStructuredBuffer<ParticleForGPU> gParticles : register(u0);
RWStructuredBuffer<EmitterState> gEmitterState : register(u1);

cbuffer CSParams : register(b0)
{
    float DeltaTime;
    uint NumParticles;
    uint FrameIndex;
    float4x4 CameraViewProj;
}

cbuffer EmitterParam : register(b2)
{
    float3 position;
    float emitRate; // 秒間発生数

    uint emitCount;
    float emitInterval; // SEQUENTIAL用

    float3 direction;
    float angle;

    float speedMin, speedMax;
    float lifeMin, lifeMax;

    float4 color;
};

float4x4 MakeTranslation(float3 t)
{
    return float4x4(
        1, 0, 0, 0,
        0, 1, 0, 0,
        0, 0, 1, 0,
        t.x, t.y, t.z, 1
    );
}

float4x4 MakeScale(float sx, float sy, float sz)
{
    return float4x4(
        sx, 0, 0, 0,
        0, sy, 0, 0,
        0, 0, sz, 0,
        0, 0, 0, 1
    );
}

float RandomFloat(uint seed)
{
    seed = (seed << 13) ^ seed;
    return frac((seed * (seed * seed * 15731 + 789221) + 1376312589) * 0.0000000001);
}

float3 RotateDirection2D(float3 dir, float angle, uint seed)
{
    float randAngle = (RandomFloat(seed) * 2.0f - 1.0f) * angle;
    float s = sin(randAngle);
    float c = cos(randAngle);
    float3 newDir;
    newDir.x = dir.x * c - dir.y * s;
    newDir.y = dir.x * s + dir.y * c;
    newDir.z = dir.z;
    return normalize(newDir);
}

[numthreads(256, 1, 1)]
void main(uint3 DTid : SV_DispatchThreadID)
{
    uint id = DTid.x;
    if (id >= NumParticles)
        return;

    // =============================
    // Emit対象決定 (スレッド0だけ)
    // =============================
    if (id == 0)
    {
        EmitterState s = gEmitterState[0];

    // =============================
    // ONESHOT
    // =============================
        if (s.emitMode == 2)
        {
            uint emitted = 0;
            for (uint i = 0; i < NumParticles; i++)
            {
                if (emitted >= s.emitRemaining)
                    break;

                if (gParticles[i].alive == 0)
                {
                    gParticles[i].alive = 2;
                    emitted++;
                }
            }

        // emitRemaining減少
            if (s.emitRemaining > emitted)
                s.emitRemaining -= emitted;
            else
                s.emitRemaining = 0;

            if (s.emitRemaining == 0)
                s.emitMode = 0;

            gEmitterState[0] = s;
        }

    // =============================
    // LOOP
    // =============================
        else if (s.emitMode == 1)
        {
            s.emitTimer -= DeltaTime;

            if (s.emitTimer <= 0.0f)
            {
                uint emitted = 0;
                for (uint i = 0; i < NumParticles; i++)
                {
                    if (emitted >= emitCount)
                        break;

                    if (gParticles[i].alive == 0)
                    {
                        gParticles[i].alive = 2;
                        emitted++;
                    }
                }

            // emitIntervalまで待機
                s.emitTimer = emitInterval;
            }

            gEmitterState[0] = s;
        }

    // =============================
    // SEQUENTIAL
    // =============================
        else if (s.emitMode == 3)
        {
            if (s.emitRemaining == 0)
            {
                s.emitMode = 0;
            }
            else
            {
                s.emitTimer -= DeltaTime;

                if (s.emitTimer <= 0.0f)
                {
                    uint emitted = 0;
                    for (uint i = 0; i < NumParticles; i++)
                    {
                        if (emitted >= min(emitCount, s.emitRemaining))
                            break;

                        if (gParticles[i].alive == 0)
                        {
                            gParticles[i].alive = 2;
                            emitted++;
                        }
                    }

                // emitRemainingを減らす
                    if (s.emitRemaining > emitted)
                        s.emitRemaining -= emitted;
                    else
                        s.emitRemaining = 0;

                // emitIntervalまで待機
                    s.emitTimer = emitInterval;

                    if (s.emitRemaining == 0)
                        s.emitMode = 0;
                }
            }

            gEmitterState[0] = s;
        }
    }

    GroupMemoryBarrierWithGroupSync();

    // =============================
    // 各スレッドでEmit or Update
    // =============================
    ParticleForGPU p = gParticles[id];

    // -----------------------------------------------------
    // Emit or Update
    // -----------------------------------------------------
    if (p.alive == 2)
    {
        // 新しいパーティクルを生成
        p.position = position;
        float3 dir = RotateDirection2D(direction, angle, FrameIndex * id);
        p.velocity = dir * lerp(speedMin, speedMax, frac(id * 0.13));
        p.currentTime = 0;
        p.lifeTime = lerp(lifeMin, lifeMax, frac(id * 0.37));
        p.color = color;
        p.alive = 1;
    }
    else
    {
        if (p.alive == 1)
        {
            // 生存パーティクルは寿命を進める
            p.currentTime += DeltaTime;
            if (p.currentTime >= p.lifeTime)
            {
                p.alive = 0;
            }
            else
            {
                p.position += p.velocity * DeltaTime;
            }
        }
    }

    // Transform 更新
    float4x4 T = MakeTranslation(p.position);
    float4x4 S = MakeScale(0.2, 0.2, 0.2);
    p.World = mul(S, T);
    p.WVP = mul(p.World, CameraViewProj);

    gParticles[id] = p;
}
