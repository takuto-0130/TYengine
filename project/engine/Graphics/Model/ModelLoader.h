#pragma once
#include "DirectXBasis.h"
#include "SrvManager.h"

namespace TYEngine
{
	namespace Graphics
	{

		/// <summary>
		/// モデル読み込みの基盤を提供するクラス。  
		/// DirectX の基盤および SRV 管理クラスへの参照を保持し、  
		/// ModelLoader によるモデルデータのロードを支援する。
		/// </summary>
		class ModelLoader
		{
		public: // メンバ関数
			/// <summary>
			/// 初期化処理。  
			/// DirectX 基盤および SRV マネージャを登録し、モデル読み込みの準備を行う。
			/// </summary>
			/// <param name="dxBasis">DirectX 基盤（デバイス・コマンド関連）。</param>
			/// <param name="srvManager">SRV 管理クラス。</param>
			void Initialize(Core::DirectXBasis* dxBasis, SrvManager* srvManager);

		public:
			/// <summary>
			/// 登録されている DirectX 基盤を取得する。
			/// </summary>
			/// <returns>DirectXBasis インスタンス。</returns>
			Core::DirectXBasis* GetDirectXBasis() const { return dxBasis_; }

			/// <summary>
			/// 登録されている SRV 管理クラスを取得する。
			/// </summary>
			/// <returns>SrvManager インスタンス。</returns>
			SrvManager* GetSrvManager() const { return srvManager_; }

		private:
			Core::DirectXBasis* dxBasis_ = nullptr; ///< DirectX 基盤（デバイス・コマンド関連）。
			SrvManager* srvManager_ = nullptr; ///< SRV 管理クラスへの参照。
		};

	} // namespace Graphics
} // namespace TYEngine
