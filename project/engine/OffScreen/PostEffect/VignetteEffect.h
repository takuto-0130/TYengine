#pragma once
#include "PostEffectBase.h"

/// <summary>
/// 周辺減光（ビネット）効果を適用するポストエフェクトクラス。  
/// 画面の中心から外側に向けて暗くなる効果を付与する。
/// </summary>
namespace TYEngine
{
	namespace OffScreen
	{

		/// <summary>
		/// 周辺減光（ビネット）効果を適用するポストエフェクトクラス。  
		/// 画面の中心から外側に向けて暗くなる効果を付与する。
		/// </summary>
		class VignetteEffect
			: public PostEffectBase
		{
		public:
			/// <summary>
			/// エフェクトの初期化を行う。  
			/// シェーダー・定数バッファ・パラメータの準備を行う。
			/// </summary>
			/// <param name="dx">DirectX基盤クラス（デバイス・コマンド管理）</param>
			/// <param name="srv">SRV管理クラス</param>
			void Initialize(Core::DirectXBasis* dx, Graphics::SrvManager* srv) override;

			/// <summary>
			/// 毎フレームの更新処理を行う。  
			/// エフェクトパラメータの補間や時間依存の処理などを実装可能。
			/// </summary>
			void Update() override;

			/// <summary>
			/// ImGuiによるパラメータ編集UIを表示する。  
			/// intensity / power / color の調整を行える。
			/// </summary>
			void ImGuiUpdate() override;

			/// <summary>
			/// 入力テクスチャにビネット効果を適用し、描画結果を出力する。
			/// </summary>
			/// <param name="input">入力となるレンダーターゲット</param>
			void Apply(RenderTexture* input) override;

		public:
			/// <summary>ビネット強度を設定する</summary>
			/// <param name="i">強度値（大きいほど周辺が暗くなる）</param>
			void SetIntensity(float i) { param_->intensity = i; }

			/// <summary>減衰のカーブ（パワー）を設定する</summary>
			/// <param name="p">カーブ係数（小さいほど中心部が広く明るくなる）</param>
			void SetPower(float p) { param_->power = p; }

			/// <summary>ビネットの色を設定する</summary>
			/// <param name="c">ビネットカラー（RGB）</param>
			void SetColor(const Utility::Vector3& c) { param_->vignetteColor = c; }

			/// <summary>現在のビネット強度を取得する</summary>
			float GetIntensity() const { return param_->intensity; }

			/// <summary>現在のパワー値を取得する</summary>
			float GetPower() const { return param_->power; }

			/// <summary>現在のビネットカラーを取得する</summary>
			Utility::Vector3 GetColor() const { return param_->vignetteColor; }

		private:
			/// <summary>
			/// ビネット効果用の定数バッファ構造体。  
			/// intensity：減光の強度  
			/// power：中心からの減衰カーブ  
			/// vignetteColor：周辺の色
			/// </summary>
			struct VignetteParam
			{
				float intensity = 16.0f;   ///< 減光の強度
				float power = 0.8f;        ///< 減衰カーブ
				float padding[2];          ///< アライメント用
				Utility::Vector3 vignetteColor = { 0.0f, 0.0f, 0.0f }; ///< 周辺の色
			};

			/// <summary>ビネット用のパラメータを保持する共有ポインタ</summary>
			std::shared_ptr<VignetteParam> param_;
		};

	} // namespace OffScreen
} // namespace TYEngine


