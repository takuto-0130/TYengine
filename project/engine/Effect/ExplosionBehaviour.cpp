#include "ExplosionBehaviour.h"
#include "mathFunc.h"

namespace TYEngine
{
	namespace Effect
	{

		using namespace Utility;

		void ExplosionBehaviour::Update(ParticleParam& p, float dt)
		{
			float t = p.currentTime / p.lifeTime;  // 0 → 1

			// ▼ 拡大スピード（前半素早く、後半はゆっくり）
			float expand = 1.0f + dt * (4.0f - t * 3.0f);
			p.transform.scale.x *= expand;
			p.transform.scale.y *= expand;
			// ビルボード想定ならZは影響しないことが多いが、念のため
			p.transform.scale.z *= expand;

			// ▼ 色：黄色 → オレンジ → 茶色（爆発感）
			p.color.x = 1.0f;            // R
			p.color.y = 0.8f - t * 0.5f; // G 減らす（赤みが増す）
			p.color.z = 0.3f - t * 0.3f; // B 減らす（暗くなる）
			p.color.w = 1.0f - t;        // フェードアウト

			// ▼ 速度：減速しながら飛ぶ（ショック波）
			p.transform.translate += p.velocity * dt;
			p.velocity *= (1.0f - dt * 2.5f);  // 空気抵抗でだんだん止まる
		}

	} // namespace Effect
} // namespace TYEngine
