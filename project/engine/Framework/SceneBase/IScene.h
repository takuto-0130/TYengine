#pragma once
#include "Input.h"
#include "Camera.h"


namespace TYEngine
{
	namespace Framework
	{

		class SceneManager;

		/// <summary>
		/// シーンの共通インターフェースを定義する純粋仮想クラス。  
		/// すべてのゲームシーン（タイトル・ゲームプレイ・リザルトなど）はこのクラスを継承して実装する。  
		/// 更新、描画、UI描画、初期化の共通フローを統一する目的で使用される。
		/// </summary>
		class IScene
		{
		protected:
			/// <summary>入力管理クラス（キーボード・マウス・パッドなど）。</summary>
			Input* input_ = nullptr;

			/// <summary>カメラクラス。</summary>
			TYEngine::CameraSystem::Camera* camera_ = nullptr;

			/// <summary>シーン管理クラス（シーン遷移や現在シーンの制御を行う）。</summary>
			SceneManager* sceneManager_ = nullptr;

		public:
			/// <summary>
			/// シーンの初期化処理。  
			/// リソースの読み込みやオブジェクト生成などを行う。
			/// </summary>
			virtual void Init() = 0;

			/// <summary>
			/// シーンの更新処理。  
			/// 入力や状態遷移、オブジェクトの動作を管理する。
			/// </summary>
			virtual void Update() = 0;

			/// <summary>
			/// シーンの描画処理。  
			/// モデル、スプライトなどのレンダリングを行う。
			/// </summary>
			virtual void Draw() = 0;

			/// <summary>
			/// シーン専用の UI（ImGui など）を描画する。
			/// </summary>
			virtual void UIDraw() = 0;

			/// <summary>
			/// シーンマネージャを登録する。  
			/// シーン遷移時に次シーンを指定する際に使用する。
			/// </summary>
			/// <param name="sceneManager">SceneManager のインスタンス。</param>
			virtual void SetSceneManager(SceneManager* sceneManager) { sceneManager_ = sceneManager; }

			/// <summary>
			/// カメラを設定する。
			/// </summary>
			/// <param name="camera">使用するカメラインスタンス。</param>
			void SetCamera(CameraSystem::Camera* camera) { camera_ = camera; };

			/// <summary>仮想デストラクタ。</summary>
			virtual ~IScene() = default;
		};

	} // namespace Framework
} // namespace TYEngine
