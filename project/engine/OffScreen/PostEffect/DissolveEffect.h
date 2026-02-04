#pragma once
#include "PostEffectBase.h"

/// <summary>
/// ディゾルブ（溶解）効果を適用するポストエフェクトクラス。  
/// マスクテクスチャやしきい値を用いて、オブジェクトが徐々に消滅・出現する演出を行う。
/// </summary>
namespace TYEngine
{
	namespace OffScreen
	{

		/// <summary>
		/// ディゾルブ（溶解）効果を適用するポストエフェクトクラス。  
		/// マスクテクスチャやしきい値を用いて、オブジェクトが徐々に消滅・出現する演出を行う。
		/// </summary>
		class DissolveEffect :
			public PostEffectBase
		{
		public:
			/// <summary>
			/// 初期化処理。  
			/// シェーダー、定数バッファ、マスクテクスチャなどの初期セットアップを行う。
			/// </summary>
			/// <param name="dx">DirectX 基盤。</param>
			/// <param name="srv">SRV 管理クラス。</param>
			void Initialize(Core::DirectXBasis* dx, Graphics::SrvManager* srv) override;

			/// <summary>
			/// フレームごとの更新処理。  
			/// 時間経過によるしきい値の変化などを反映する。
			/// </summary>
			void Update() override;

			/// <summary>
			/// ImGui 上でパラメータ編集 UI を表示する。  
			/// しきい値やエッジカラーの調整が可能。
			/// </summary>
			void ImGuiUpdate() override;

			/// <summary>
			/// ディゾルブ効果を入力テクスチャに適用する。  
			/// しきい値に基づいてピクセルをマスクし、消失または出現させる。
			/// </summary>
			/// <param name="input">処理対象のレンダーターゲット。</param>
			void Apply(RenderTexture* input) override;

		public:
			// ========================
			//        Setter
			// ========================

			/// <summary>ディゾルブ進行度（しきい値）を設定する。</summary>
			/// <param name="t">しきい値（0.0で未開始、1.0で完全消滅）。</param>
			void SetThreshold(float t) { param_->threshold = t; }

			/// <summary>エッジ表示を有効または無効に設定する。</summary>
			/// <param name="use">true で有効、false で無効。</param>
			void SetUseEdge(bool use) { param_->useEdge = use ? 1 : 0; }

			/// <summary>エッジ部分の発光色を設定する。</summary>
			/// <param name="color">エッジカラー。</param>
			void SetEdgeColor(const Utility::Vector3& color) { param_->edgeColor = color; }

			// ========================
			//        Getter
			// ========================

			/// <summary>現在のディゾルブ進行度（しきい値）を取得する。</summary>
			float GetThreshold() const { return param_->threshold; }

			/// <summary>エッジ表示が有効かどうかを取得する。</summary>
			bool GetUseEdge() const { return param_->useEdge != 0; }

			/// <summary>現在のエッジカラーを取得する。</summary>
			Utility::Vector3 GetEdgeColor() const { return param_->edgeColor; }

		private:
			/// <summary>
			/// ディゾルブ効果に使用される定数バッファ構造体。  
			/// しきい値・エッジ表示フラグ・エッジカラーなどを保持する。
			/// </summary>
			struct DissolveParam
			{
				float threshold = 0.0f;                      ///< ディゾルブ進行度（0～1）。
				int useEdge = 0;                             ///< エッジ描画の有無（1=有効, 0=無効）。
				float padding[2];                            ///< アライメント調整用パディング。
				Utility::Vector3 edgeColor = { 0.5f, 0.5f, 0.5f };    ///< エッジの発光カラー。
			};

			/// <summary>ディゾルブ用パラメータ。</summary>
			std::shared_ptr<DissolveParam> param_;
		};

	} // namespace OffScreen
} // namespace TYEngine
