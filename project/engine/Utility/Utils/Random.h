#pragma once
#include <random>
#include "SingletonObject.h"

namespace TYEngine
{
	namespace Utility
	{

		/// <summary>
		/// 乱数生成クラス（シングルトン）。
		/// メルセンヌ・ツイスタ（std::mt19937）を使用した高品質な乱数を提供する。
		/// </summary>
		class Random :
			public SingletonObject<Random>
		{
			friend class SingletonObject<Random>;
			friend struct std::default_delete<Random>;

		private:
			// 外部からの new/delete を禁止
			Random()
			{
				std::random_device rd;
				mt_.seed(rd());
			}
			~Random() = default;

		public:
			/// <summary>指定範囲の整数乱数を取得する（min <= val <= max）。</summary>
			int Int(int min, int max)
			{
				std::uniform_int_distribution<int> dist(min, max);
				return dist(mt_);
			}

			/// <summary>0 から 9 までの整数乱数を取得する。</summary>
			int Int09()
			{
				std::uniform_int_distribution<int> dist(0, 9);
				return dist(mt_);
			}

			/// <summary>指定範囲の浮動小数点乱数を取得する（min <= val < max）。</summary>
			float Float(float min, float max)
			{
				std::uniform_real_distribution<float> dist(min, max);
				return dist(mt_);
			}

			/// <summary>0.0 から 1.0 までの浮動小数点乱数を取得する。</summary>
			float Float01()
			{
				std::uniform_real_distribution<float> dist(0.0f, 1.0f);
				return dist(mt_);
			}

		private:
			/// <summary>メルセンヌ・ツイスタ乱数生成器。</summary>
			std::mt19937 mt_;
		};

	} // namespace Utility
} // namespace TYEngine
