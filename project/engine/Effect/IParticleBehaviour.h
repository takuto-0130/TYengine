#pragma once
#include "ParticleParam.h"

namespace TYEngine
{
	namespace Effect
	{

		/// <summary>
		/// パーティクルの挙動（振る舞い）を定義するインターフェース。
		/// Strategyパターンにより、移動や変化のロジックを切り替えるために使用する。
		/// </summary>
		class IParticleBehaviour
		{
		public:
			virtual ~IParticleBehaviour() = default;

			/// <summary>
			/// パーティクルの更新処理を行う。
			/// </summary>
			/// <param name="p">更新対象のパーティクルパラメータ。</param>
			/// <param name="dt">デルタタイム。</param>
			virtual void Update(ParticleParam& p, float dt) = 0;
		};

	} // namespace Effect
} // namespace TYEngine
