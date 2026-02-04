#pragma once
#include "PostEffect/IPostEffect.h"
#include "OutlinePass.h"
#include "DoFPass.h"
#include "CopyImage.h"
#include "SingletonObject.h"
#include <vector>
#include <memory>

namespace TYEngine
{
	namespace Core
	{
		class DirectXBasis;
	}
	namespace Graphics
	{
		class SrvManager;
	}
}

namespace TYEngine
{
	namespace OffScreen
	{

		/// <summary>
		/// ポストエフェクト全体を統括管理するマネージャークラス。
		/// 複数のポストエフェクト（Outline, DoF, Copy など）を登録・適用・制御する。
		/// </summary>
		class PostEffectManager :
			public Utility::SingletonObject<PostEffectManager>
		{
			friend class Utility::SingletonObject<PostEffectManager>;
			friend struct std::default_delete<PostEffectManager>;

		private:
			// 外部からの new/delete を禁止
			PostEffectManager() = default;
			~PostEffectManager() = default;

		public:
			/// <summary>
			/// 初期化処理。DirectX 基盤と SRV 管理を登録する。
			/// </summary>
			/// <param name="dx">DirectX 基盤。</param>
			/// <param name="srv">SRV マネージャ。</param>
			void Initialize(Core::DirectXBasis* dx, Graphics::SrvManager* srv);

			/// <summary>
			/// ポストエフェクトを追加する。
			/// </summary>
			/// <param name="name">エフェクト名。</param>
			/// <param name="effect">登録するポストエフェクト（IPostEffect の派生）。</param>
			void AddEffect(const std::string& name, std::shared_ptr<IPostEffect> effect_);

			/// <summary>
			/// すべてのエフェクトの更新処理を行う。
			/// </summary>
			void Update();

			/// <summary>
			/// アウトラインと被写界深度（DoF）を適用する。
			/// </summary>
			/// <param name="source">入力テクスチャ。</param>
			/// <returns>処理後のレンダーターゲット。</returns>
			RenderTexture* ApplyOutlineAndDoF(RenderTexture* source);

			/// <summary>
			/// 登録されたポストエフェクトを順に適用する。
			/// </summary>
			/// <param name="source">入力テクスチャ。</param>
			/// <param name="target">出力テクスチャ。</param>
			void Apply(RenderTexture* source, RenderTexture* target);

			/// <summary>
			/// 一時的なレンダーターゲットを設定する（ping-pong処理用）。
			/// </summary>
			/// <param name="rt">設定する RenderTexture。</param>
			void SetTempRenderTexture(std::unique_ptr<RenderTexture> rt);

			/// <summary>
			/// アウトライン描画用のレンダーターゲットを設定する。
			/// </summary>
			/// <param name="rt">設定する RenderTexture。</param>
			void SetOutlineRenderTexture(std::unique_ptr<RenderTexture> rt);

			/// <summary>
			/// 特定のエフェクトの有効・無効を設定する。
			/// </summary>
			/// <param name="name">エフェクト名。</param>
			/// <param name="enabled">有効にするなら true。</param>
			void SetEffectEnabled(const std::string& name, bool enabled);

			/// <summary>
			/// エフェクトの描画順序を変更する。
			/// </summary>
			/// <param name="name">エフェクト名。</param>
			/// <param name="newIndex">新しいインデックス位置。</param>
			void MoveEffect(const std::string& name, int newIndex);

			/// <summary>
			/// アウトラインエフェクトの有効・無効を設定する。
			/// </summary>
			/// <param name="enabled">有効にするなら true。</param>
			void SetOutlineEnabled(bool enabled) { enabledOutline_ = enabled; }

			/// <summary>
			/// 被写界深度（DoF）の有効・無効を設定する。
			/// </summary>
			/// <param name="enabled">有効にするなら true。</param>
			void SetDoFEnabled(bool enabled) { enabledDoF_ = enabled; }

			/// <summary>
			/// 登録されている全てのエフェクトを無効化する。
			/// </summary>
			void EffectAllDisable();

			/// <summary>
			/// 指定した名前のエフェクトを型付きで取得する。
			/// </summary>
			/// <typeparam name="T">取得したいエフェクト型（例：OutlinePass, DoFPass）。</typeparam>
			/// <param name="name">エフェクト名。</param>
			/// <returns>該当するエフェクト。存在しない場合 nullptr。</returns>
			template<typename T>
			T* GetEffect(const std::string& name)
			{
				for (auto& e : effectStack_)
				{
					if (e.name == name)
					{
						return dynamic_cast<T*>(e.effect_.get());
					}
				}
				return nullptr;
			}

		private:
			Core::DirectXBasis* dxBasis_ = nullptr;   ///< DirectX 基盤ポインタ。
			Graphics::SrvManager* srvMgr_ = nullptr;      ///< SRV 管理クラスへの参照。

			/// <summary>
			/// エフェクト情報構造体。
			/// </summary>
			struct EffectEntry
			{
				std::string name;                     ///< エフェクト名。
				std::shared_ptr<IPostEffect> effect_;  ///< エフェクト本体。
			};

			std::vector<EffectEntry> effectStack_;      ///< 適用順に並ぶエフェクトスタック。

			std::unique_ptr<RenderTexture> tempRt_;     ///< 一時的なレンダーターゲット（ping-pong用）。
			std::unique_ptr<RenderTexture> outlineRt_;  ///< アウトライン用のレンダーターゲット。

			std::unique_ptr<OutlinePass> outlinePass;   ///< アウトライン描画パス。
			bool enabledOutline_ = true;                ///< アウトライン有効フラグ。

			std::unique_ptr<DoFPass> dofPass;           ///< 被写界深度エフェクト。
			bool enabledDoF_ = true;                    ///< DoF有効フラグ。

			std::unique_ptr<CopyImageEffect> copyImage_; ///< コピーエフェクト（最終出力用）。
		};

	} // namespace OffScreen
} // namespace TYEngine


