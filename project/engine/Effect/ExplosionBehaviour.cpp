#include "ExplosionBehaviour.h"
#include "mathFunc.h"

void ExplosionBehaviour::Update(ParticleParam& p, float dt)
{
    float t = p.currentTime / p.lifeTime;  // 0 → 1

    // ▼ 拡大スピード（前半素早く、後半はゆっくり）
    float expand = 1.0f + dt * (4.0f - t * 3.0f);
    p.transform.scale.x *= expand;
    p.transform.scale.y *= expand;

    // ▼ 色：黄色 → オレンジ → 茶色（爆発感）
    p.color.x = 1.0f;            // R
    p.color.y = 0.8f - t * 0.5f; // G 減らす
    p.color.z = 0.3f - t * 0.3f; // B 減らす
    p.color.w = 1.0f - t;        // フェードアウト

    // ▼ 速度：減速しながら飛ぶ（ショック波）
    p.transform.translate += p.velocity * dt;
    p.velocity *= (1.0f - dt * 2.5f);  // だんだん止まる
}
