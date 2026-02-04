#pragma once
#include "IParticleBehaviour.h"
#include "ParticleParam.h"

namespace TYEngine
{
	namespace Effect
	{

		/// <summary>
		/// 飛行機雲（コントレイル）用の挙動クラス。
		/// 放出後にその場に留まりながら拡散・消失する動きを定義する。
		/// </summary>
		class ContrailBehaviour : public IParticleBehaviour
		{
		public:
			/// <summary>
			/// パーティクルの更新（コントレイル挙動の適用）。
			/// </summary>
			void Update(ParticleParam& p, float dt) override;
		};

	} // namespace Effect
} // namespace TYEngine
