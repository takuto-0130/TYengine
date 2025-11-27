#include "DebrisBehaviour.h"
#include "mathFunc.h"

void DebrisBehaviour::Update(ParticleParam& p, float dt)
{
    float t = p.currentTime / p.lifeTime;  // 0 → 1

    // ▼ 速度で飛ぶ
    p.transform.translate += p.velocity * dt;

    // ▼ 空気抵抗（減速）
    p.velocity *= (1.0f - dt * 3.0f);  // 3.0f は抵抗の強さ

    // ▼ 重力（お好みでON/OFF）
    p.velocity.y -= 9.8f * 0.3f * dt;  // 弱めの重力

    // ▼ 火花の色変化（StarFox風）
    p.color.x = 1.0f;           // R
    p.color.y = 0.8f - t * 0.6f; // G: 0.8 → 0.2
    p.color.z = 0.2f - t * 0.2f; // B: 0.2 → 0.0（黒）
    p.color.w = 1.0f - t;        // Alpha フェードアウト

    // ▼ 破片は少しずつ小さくなる
    float shrink = 1.0f - dt * 1.8f;
    p.transform.scale *= shrink;
}
