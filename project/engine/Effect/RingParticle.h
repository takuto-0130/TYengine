#pragma once
#include "IParticleRenderer.h"

namespace TYEngine
{
	namespace Effect
	{

		/// <summary>
		/// リング形状のパーティクルレンダラー。
		/// 衝撃波やエフェクトのアクセントとしてリング状のメッシュを描画する。
		/// </summary>
		class RingParticle : public IParticleRenderer
		{
		protected:
			void CreateResources() override;
			ParticleParam MakeNewParticle(std::mt19937& random, const Emitter& emitter) override;
			std::list<ParticleParam> Emit(std::mt19937& random)override;
		};

	} // namespace Effect
} // namespace TYEngine

