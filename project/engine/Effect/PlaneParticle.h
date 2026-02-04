#pragma once
#include "IParticleRenderer.h"

namespace TYEngine
{
	namespace Effect
	{

		/// <summary>
		/// 板ポリゴンを使用した標準的なパーティクルレンダラー。
		/// </summary>
		class PlaneParticle : public IParticleRenderer
		{
		public:
		private:
			void CreateResources() override;
			ParticleParam MakeNewParticle(std::mt19937& random, const Emitter& emitter) override;
		};

	} // namespace Effect
} // namespace TYEngine
