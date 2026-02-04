#pragma once
#include "PostEffectBase.h"

/// <summary>
/// ガウシアンブラー（Gaussian Blur）効果を適用するポストエフェクトクラス。  
/// 画像をぼかすことで、被写界深度や柔らかな光の表現などに使用される。
/// </summary>
namespace TYEngine
{
	namespace OffScreen
	{

		/// <summary>
		/// ガウシアンブラー（Gaussian Blur）効果を適用するポストエフェクトクラス。  
		/// 画像をぼかすことで、被写界深度や柔らかな光の表現などに使用される。
		/// </summary>
		class GaussianEffect :
			public PostEffectBase
		{
		public:
			/// <summary>
			/// 初期化処理。  
			/// DirectX リソースおよび SRV 管理をセットアップする。
			/// </summary>
			/// <param name="dx">DirectX 基盤。</param>
			/// <param name="srv">SRV 管理クラス。</param>
			void Initialize(Core::DirectXBasis* dx, Graphics::SrvManager* srv) override;

			/// <summary>
			/// 毎フレームの更新処理。  
			/// 内部パラメータの補間やアニメーション的な変化を反映する。
			/// </summary>
			void Update() override;

			/// <summary>
			/// ImGui によるデバッグ UI 表示。  
			/// カーネルサイズやシグマ値をリアルタイムで調整可能。
			/// </summary>
			void ImGuiUpdate() override;

			/// <summary>
			/// ガウシアンブラーを適用し、入力テクスチャをぼかす。
			/// </summary>
			/// <param name="input">処理対象のレンダーターゲット。</param>
			void Apply(RenderTexture* input) override;

		public:
			/// <summary>
			/// ブラー処理に使用するカーネルサイズを設定する。  
			/// 奇数値を推奨（例：3, 5, 7）。大きいほど広範囲にぼける。
			/// </summary>
			/// <param name="size">カーネルサイズ。</param>
			void SetKernelSize(int size) { param_->kernelSize = size; }

			/// <summary>
			/// ブラーのシグマ値（分散）を設定する。  
			/// 値が大きいほどぼけが強くなる。
			/// </summary>
			/// <param name="sigma">シグマ値。</param>
			void SetSigma(float sigma) { param_->sigma = sigma; }

			/// <summary>現在設定されているカーネルサイズを取得する。</summary>
			int GetKernelSize() const { return param_->kernelSize; }

			/// <summary>現在設定されているシグマ値を取得する。</summary>
			float GetSigma() const { return param_->sigma; }

		private:
			/// <summary>
			/// ガウシアンブラー効果用のパラメータ構造体。  
			/// カーネルサイズとシグマ値を GPU に転送してブラー強度を制御する。
			/// </summary>
			struct GaussianParam
			{
				int kernelSize = 3;   ///< カーネルサイズ（サンプル数）。
				float sigma = 5.0f;   ///< シグマ値（ブラーの強さ）。
			};

			/// <summary>ブラー処理用のパラメータ。</summary>
			std::shared_ptr<GaussianParam> param_;
		};

	} // namespace OffScreen
} // namespace TYEngine

