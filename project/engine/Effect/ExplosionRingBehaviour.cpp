#include "ExplosionRingBehaviour.h"
#include "MathFunc.h"

namespace TYEngine
{
	namespace Effect
	{

		using namespace Utility;

		void ExplosionRingBehaviour::Update(ParticleParam& p, float dt)
		{
			float t = p.currentTime / p.lifeTime; // 0 → 1

			// ▼ リングの横方向だけ急膨張
			float expand = 1.0f + dt * (10.0f - t * 6.0f);
			p.transform.scale.x *= expand; // 横拡大
			p.transform.scale.z *= expand; // 奥行き拡大（RingParticle 用）

			// ▼ 高さはほぼ一定（横だけ伸びるリング）
			p.transform.scale.y *= (1.0f + dt * 0.1f);

			// ▼ 色調：最初は明るい黄色 → 薄いオレンジ → 消失
			p.color.x = 1.0f;               // R
			p.color.y = 0.9f - t * 0.4f;    // G: 0.9 → 0.5
			p.color.z = 0.2f;               // B: 固定
			p.color.w = 1.0f - t;           // Alpha フェードアウト

			// ▼ 中心からの微弱な広がり（ほぼ固定）
			p.transform.translate += p.velocity * (0.2f * dt);

			// ▼ 速度は急速に減衰（リングは飛ばず、その場で広がる）
			p.velocity *= (1.0f - dt * 4.0f);
		}

	} // namespace Effect
} // namespace TYEngine
