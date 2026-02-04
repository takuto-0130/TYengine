#pragma once
#include "CopyPass.h"
#include "RenderTexture.h"

class DirectXBasis;
class SrvManager;

namespace TYEngine
{
	namespace OffScreen
	{

		/// <summary>
		/// ポストエフェクト共通インターフェース。  
		/// 各種ポストプロセスエフェクト（ブラー・アウトライン・トーンマッピングなど）を  
		/// 統一的に扱うための基底クラス。
		/// </summary>
		class IPostEffect
		{
		public:
			/// <summary>デストラクタ。</summary>
			virtual ~IPostEffect() = default;

			/// <summary>
			/// 初期化処理を行う。  
			/// DirectX の基盤および SRV 管理クラスを用いてリソースを準備する。
			/// </summary>
			/// <param name="dx">DirectX 基盤。</param>
			/// <param name="srv">SRV 管理クラス。</param>
			virtual void Initialize(Core::DirectXBasis* dx, Graphics::SrvManager* srv) = 0;

			/// <summary>
			/// 毎フレームの更新処理。  
			/// エフェクト内部の状態や補間を更新する。
			/// </summary>
			virtual void Update() = 0;

			/// <summary>
			/// ImGui 上でパラメータを編集できるようにする。  
			/// デバッグ・チューニング用の UI 表示処理を行う。
			/// </summary>
			virtual void ImGuiUpdate() = 0;

			/// <summary>
			/// ポストエフェクトを適用する。  
			/// 入力テクスチャに対して処理を行い、結果を出力する。
			/// </summary>
			/// <param name="input">処理対象のレンダーターゲット。</param>
			virtual void Apply(RenderTexture* input) = 0;

			/// <summary>
			/// このエフェクトが有効かどうかを取得する。
			/// </summary>
			/// <returns>有効なら true、無効なら false。</returns>
			virtual bool IsEnabled() const = 0;

			/// <summary>
			/// エフェクトの有効・無効を設定する。
			/// </summary>
			/// <param name="enabled">true で有効、false で無効。</param>
			virtual void SetEnabled(bool enabled) = 0;
		};

	} // namespace OffScreen
} // namespace TYEngine

