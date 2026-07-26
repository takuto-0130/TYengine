#pragma once
#include "IParticleBehaviour.h"
#include "ParticleParam.h"

namespace TYEngine
{
	namespace Effect
	{

		/// <summary>
		/// 爆発時のリング状（衝撃波）パーティクルの急速拡大・アルファ減衰挙動を更新するビヘイビアクラス。
		/// </summary>
		class ExplosionRingBehaviour : public IParticleBehaviour
		{
		public:
			void Update(ParticleParam& p, float dt) override;
		};

	} // namespace Effect
} // namespace TYEngine
