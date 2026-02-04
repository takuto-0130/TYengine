#pragma once
#include "PostEffectBase.h"

/// <summary>
/// 輝度差をもとにエッジ（輪郭）を検出し、指定した色で描画するポストエフェクト。  
/// 深度ベースではなく、画面上の明暗差に基づくアウトライン効果を実現する。
/// </summary>
namespace TYEngine
{
	namespace OffScreen
	{

		/// <summary>
		/// 輝度差をもとにエッジ（輪郭）を検出し、指定した色で描画するポストエフェクト。  
		/// 深度ベースではなく、画面上の明暗差に基づくアウトライン効果を実現する。
		/// </summary>
		class LuminanceBasedOutlineEffect :
			public PostEffectBase
		{
		public:
			/// <summary>
			/// エフェクトの初期化処理。  
			/// 必要なシェーダ・定数バッファなどをセットアップする。
			/// </summary>
			/// <param name="dx">DirectX 基盤。</param>
			/// <param name="srv">SRV 管理クラス。</param>
			void Initialize(Core::DirectXBasis* dx, Graphics::SrvManager* srv) override;

			/// <summary>
			/// エフェクトの更新処理。  
			/// 内部パラメータの補間や時間経過による調整を行う。
			/// </summary>
			void Update() override;

			/// <summary>
			/// ImGui 上でパラメータをリアルタイム調整するための UI を表示する。
			/// </summary>
			void ImGuiUpdate() override;

			/// <summary>
			/// エフェクトを適用する。  
			/// 入力テクスチャの輝度差をもとに輪郭を検出し、指定色で描画する。
			/// </summary>
			/// <param name="input">入力レンダーターゲット。</param>
			void Apply(RenderTexture* input) override;

		public:
			// ========================
			//        Setter
			// ========================

			/// <summary>エッジ検出の輝度閾値を設定する。</summary>
			/// <param name="t">閾値（小さいほど細部まで検出）。</param>
			void SetThreshold(float t) { param_->threshold = t; }

			/// <summary>エッジ線の太さを設定する。</summary>
			/// <param name="w">エッジの幅。</param>
			void SetEdgeWidth(float w) { param_->edgeWidth = w; }

			/// <summary>エッジの強度（コントラスト）を設定する。</summary>
			/// <param name="i">エッジの強調度。</param>
			void SetEdgeIntensity(float i) { param_->edgeIntensity = i; }

			/// <summary>エッジの色を設定する。</summary>
			/// <param name="c">エッジカラー。</param>
			void SetEdgeColor(const Utility::Vector3& c) { param_->edgeColor = c; }

			// ========================
			//        Getter
			// ========================

			/// <summary>エッジ検出の輝度閾値を取得する。</summary>
			float GetThreshold() const { return param_->threshold; }

			/// <summary>エッジ線の太さを取得する。</summary>
			float GetEdgeWidth() const { return param_->edgeWidth; }

			/// <summary>エッジの強度（コントラスト）を取得する。</summary>
			float GetEdgeIntensity() const { return param_->edgeIntensity; }

			/// <summary>エッジの色を取得する。</summary>
			Utility::Vector3 GetEdgeColor() const { return param_->edgeColor; }

		private:
			/// <summary>
			/// 輝度ベースのアウトライン描画に使用される定数バッファ構造体。  
			/// 閾値・幅・強度・色などの情報を GPU に転送する。
			/// </summary>
			struct LuminanceOutlineParam
			{
				float threshold = 0.1f;                  ///< 輝度差によるエッジ検出の閾値。
				float edgeWidth = 1.0f;                  ///< エッジ線の幅。
				float edgeIntensity = 1.0f;              ///< エッジ強度（コントラスト量）。
				float padding;                           ///< アライメント調整用パディング。
				Utility::Vector3 edgeColor = { 0.0f, 0.0f, 0.0f };///< エッジカラー。
			};

			std::shared_ptr<LuminanceOutlineParam> param_; ///< エフェクト用パラメータ。
		};

	} // namespace OffScreen
} // namespace TYEngine


