#pragma once
#include "IPostEffect.h"

/// <summary>
/// ポストエフェクトの共通基底クラス。  
/// 各ポストエフェクト（Vignette、RadialBlur、RandomEffect など）はこのクラスを継承して実装する。  
/// 有効・無効の切り替えや共通のコピー処理（CopyPass）を保持する。
/// </summary>
namespace TYEngine
{
	namespace OffScreen
	{

		/// <summary>
		/// ポストエフェクトの共通基底クラス。  
		/// 各ポストエフェクト（Vignette、RadialBlur、RandomEffect など）はこのクラスを継承して実装する。  
		/// 有効・無効の切り替えや共通のコピー処理（CopyPass）を保持する。
		/// </summary>
		class PostEffectBase
			: public IPostEffect
		{
		public:
			/// <summary>
			/// エフェクトが有効かどうかを取得する。
			/// </summary>
			/// <returns>有効なら true、無効なら false。</returns>
			bool IsEnabled() const override { return enabled_; }

			/// <summary>
			/// エフェクトの有効・無効を設定する。
			/// </summary>
			/// <param name="enabled">true で有効、false で無効。</param>
			void SetEnabled(bool enabled) override { enabled_ = enabled; }

			/// <summary>
			/// 内部で使用する CopyPass を取得する。  
			/// CopyPass はポストエフェクト結果をレンダーターゲット間で転送するための仕組み。
			/// </summary>
			/// <returns>CopyPass オブジェクト。</returns>
			CopyPass GetCopyPass() const { return copyPass_; }

		protected:
			bool enabled_ = true;          ///< エフェクトの有効／無効フラグ。
			Core::DirectXBasis* dx_ = nullptr;   ///< DirectX 基盤クラスへの参照。
			CopyPass copyPass_;            ///< 描画転送や共通処理に用いる CopyPass インスタンス。
		};

	} // namespace OffScreen
} // namespace TYEngine

