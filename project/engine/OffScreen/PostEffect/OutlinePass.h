#pragma once
#include "CopyPass.h"
#include "SrvManager.h"

/// <summary>
/// 深度情報をもとに輪郭線（アウトライン）を描画するポストエフェクトパス。  
/// 深度差分によるエッジ検出を行い、指定した色と幅で輪郭を描画する。
/// </summary>
namespace TYEngine
{
	namespace OffScreen
	{

		/// <summary>
		/// 深度情報をもとに輪郭線（アウトライン）を描画するポストエフェクトパス。  
		/// 深度差分によるエッジ検出を行い、指定した色と幅で輪郭を描画する。
		/// </summary>
		class OutlinePass
		{
		public:
			/// <summary>
			/// アウトライン描画用パスを初期化する。  
			/// 必要なシェーダ、定数バッファ、SRV 等のセットアップを行う。
			/// </summary>
			/// <param name="dx">DirectX 基盤。</param>
			/// <param name="srvMgr">SRV 管理クラス。</param>
			void Initialize(Core::DirectXBasis* dx, Graphics::SrvManager* srv);

			/// <summary>
			/// アウトライン描画に関する内部更新処理を行う。  
			/// 各種パラメータの更新やタイマー処理などを行う。
			/// </summary>
			void Update();

			/// <summary>
			/// アウトライン描画処理を実行する。  
			/// 入力テクスチャ（SRV）をもとに、エッジ検出を行い輪郭を描く。
			/// </summary>
			/// <param name="cmdList">コマンドリスト。</param>
			/// <param name="inputSrv">入力テクスチャの GPU SRV ハンドル。</param>
			void Draw(ID3D12GraphicsCommandList* cmdList, D3D12_GPU_DESCRIPTOR_HANDLE inputSrv);

			/// <summary>
			/// 深度テクスチャ（SRV）を設定する。  
			/// アウトライン検出に使用される深度情報をバインドする。
			/// </summary>
			/// <param name="handle">深度テクスチャの GPU SRV ハンドル。</param>
			void SetDepthSrv(D3D12_GPU_DESCRIPTOR_HANDLE handle);

			/// <summary>
			/// ImGui 上でのデバッグパラメータ操作。  
			/// 閾値や色、幅などをリアルタイムで調整可能。
			/// </summary>
			void ImGuiUpdate();

		public:
			// ========================
			//        Setter
			// ========================

			/// <summary>深度差による輪郭検出の閾値を設定する。</summary>
			/// <param name="t">閾値（小さいほど細かく検出）。</param>
			void SetThreshold(float t) { outlineParam_->depthThreshold = t; }

			/// <summary>輪郭線の太さを設定する。</summary>
			/// <param name="w">輪郭の幅。</param>
			void SetOutlineWidth(float w) { outlineParam_->outlineWidth = w; }

			/// <summary>テクセルサイズ（1/画面解像度）を設定する。</summary>
			/// <param name="texel">テクセルサイズ。</param>
			void SetTexelSize(const Utility::Vector2& texel) { outlineParam_->texelSize = texel; }

			/// <summary>輪郭線の色を設定する。</summary>
			/// <param name="color">輪郭線のカラー。</param>
			void SetOutlineColor(const Utility::Vector3& color) { outlineParam_->outlineColor = color; }

			// ========================
			//        Getter
			// ========================

			/// <summary>深度差による輪郭検出の閾値を取得する。</summary>
			float GetThreshold() const { return outlineParam_->depthThreshold; }

			/// <summary>輪郭線の太さを取得する。</summary>
			float GetOutlineWidth() const { return outlineParam_->outlineWidth; }

			/// <summary>テクセルサイズ（1/画面解像度）を取得する。</summary>
			Utility::Vector2 GetTexelSize() const { return outlineParam_->texelSize; }

			/// <summary>輪郭線の色を取得する。</summary>
			Utility::Vector3 GetOutlineColor() const { return outlineParam_->outlineColor; }

		private:
			/// <summary>
			/// アウトライン描画に使用される定数バッファ構造体。  
			/// 解像度・閾値・色・幅などを GPU に転送する。
			/// </summary>
			struct OutlinePassParam
			{
				Utility::Vector2 texelSize =
				{
					1.0f / Core::WindowsApp::kClientWidth, 1.0f / Core::WindowsApp::kClientHeight
				};														///< レンダリングターゲットの解像度に基づくテクセルサイズ。
				float depthThreshold = 0.001f;							///< 深度差によるエッジ検出の閾値。
				float pad0;												///< アライメント用パディング。
				Utility::Vector3 outlineColor = { 0.0f, 0.0f, 0.0f };			///< 輪郭線の色。
				float outlineWidth = 1.0f;								///< 輪郭線の太さ。
				float pad1[3] = {};										///< アライメント用パディング。
			};

			std::shared_ptr<OutlinePassParam> outlineParam_;	///< アウトライン描画用パラメータ。

			CopyPass copyPass_;	///< アウトライン描画に使用するコピー処理（描画転送を担当）。
		};

	} // namespace OffScreen
} // namespace TYEngine


