#pragma once
#include "IScene.h"
#include "StateMachine.h"
#include "State.h"

#include "ParticleManager.h"
#include "../../Stage/StageManager.h"
#include "ObjectCubemap.h"
#include "Result/ConfettiSystem.h"
#include "UISpriteColliderManager.h"
#include "../../AppSystem/Audio/GameAudio.h"

#define JSONMGR_WITH_IMGUI
#include "Utils/Json/JsonManager.h"

#include <memory>
#include <vector>

class Audio;
class RailManager;
class Player;

namespace TYEngine::Utility
{
	class BulletTimeController;
}

enum class GameSceneState {
	LOAD,		// データのロード（ロードに時間がかかる場合に使用、使わなければのちに削除）
	FADE_IN,	// シーン開始時のフェードイン
	READY,		// ゲーム開始直前の演出（不要なら消す）
	PLAY,		// 通常のゲーム進行
	PAUSE,		// 一時停止状態（メニューなど）
	DEAD,		// プレイヤーが死んだ時
	CLEAR,		// クリア演出時
	RESULT,		// リザルト（ゲーム終了後のスコア表示など）
	RETRY,		// リトライ待機中
	FADE_OUT,	// シーン遷移時のフェードアウト
	DEBUG_EDIT,	// エディタ専用
};

// 前方宣言
class GameSceneStateLoad;
class GameSceneStateFadeIn;
class GameSceneStateReady;
class GameSceneStatePlay;
class GameSceneStatePause;
class GameSceneStateDead;
class GameSceneStateClear;
class GameSceneStateResult;
class GameSceneStateRetry;
class GameSceneStateFadeOut;
class GameSceneStateDebugEdit;

/// <summary>
/// ゲームシーン
/// </summary>
class GameScene : 
	public TYEngine::Framework::IScene
{
	friend class GameSceneStateLoad;
	friend class GameSceneStateFadeIn;
	friend class GameSceneStateReady;
	friend class GameSceneStatePlay;
	friend class GameSceneStatePause;
	friend class GameSceneStateDead;
	friend class GameSceneStateClear;
	friend class GameSceneStateResult;
	friend class GameSceneStateRetry;
	friend class GameSceneStateFadeOut;
	friend class GameSceneStateDebugEdit;

public:
	using StateMachineType = TYEngine::Utility::StateMachine<GameSceneState, GameScene>;

public:
	GameScene();
	~GameScene() override;

	/// <summary>
	/// シーンの初期化処理。
	/// ステージ、プレイヤー、UI、オーディオ等の読み込みと生成を行う。
	/// </summary>
	void Init() override;

	/// <summary>
	/// シーンの毎フレーム更新処理。
	/// 進行状態（State）に応じた更新ロジックを実行する。
	/// </summary>
	void Update() override;

	/// <summary>
	/// シーンの描画処理。
	/// 背景、キャラクター、エフェクト等の3D描画を行う。
	/// </summary>
	void Draw() override;

	/// <summary>
	/// シーンのUI描画処理。
	/// スコア、HUD、メニュー画面等の2D描画を行う。
	/// </summary>
	void UIDraw() override;

private:
	/// <summary>
	/// UI関連の初期化を行う。
	/// </summary>
	void UIInit();

	/// <summary>
	/// ゲーム開始演出時のカメラワークを開始する。
	/// </summary>
	void StartCamera();

	// GameScenePlay
	/// <summary>プレイ中のUI更新。</summary>
	void PlayUIUpdate();
	/// <summary>コンボUIの更新。</summary>
	void ComboUIUpdate();

	/// <summary>
	/// パーティクルエミッターの初期設定を行う。
	/// </summary>
	void EmitterInit();

	/// <summary>
	/// デバッグ編集モードの切り替え処理。
	/// </summary>
	void SwitchEdit();

	void DebugDraw();

private: // メンバ変数
	/// <summary>カメラのオフセット位置。</summary>
	TYEngine::Utility::Vector3 cameraOffset_ = {};

	/// <summary>BGM再生ハンドル。</summary>
	int BGMHandle_ = -1;
	/// <summary>オーディオマネージャ。</summary>
	GameAudio* gameAudio_ = nullptr;

	/// <summary>ステージ管理マネージャ。</summary>
	std::unique_ptr<StageManager> stageManager_;

	/// <summary>汎用パーティクルエミッター。</summary>
	TYEngine::Effect::IParticleRenderer::Emitter emitter;
	/// <summary>リング状パーティクルエミッター。</summary>
	TYEngine::Effect::IParticleRenderer::Emitter emitterRing;

	/// <summary>UI用コライダー管理。</summary>
	TYEngine::Utility::UISpriteColliderManager uiCollider_;
	/// <summary>紙吹雪エフェクトシステム。</summary>
	ConfettiSystem confetti_;

	/// <summary>バレットタイムコントローラー。</summary>
	TYEngine::Utility::BulletTimeController* bulletTime_ = nullptr;



	/// <summary>スカイボックス。</summary>
	std::unique_ptr<TYEngine::Graphics::ObjectCubemap> skybox_;

	/// <summary>エディタ切り替えスイッチ。</summary>
	bool otherEditorSwitch_ = false;

	/// <summary>準備完了フラグ。</summary>
	bool isReady_ = true;
	/// <summary>準備カウントダウン。</summary>
	int readyCount_ = 0;
	/// <summary>開始時カメラ位置。</summary>
	TYEngine::Utility::Vector3 startCameraPos_{};
	/// <summary>開始時カメラ回転。</summary>
	TYEngine::Utility::Vector3 startCameraRot_{};
	/// <summary>開始カメラ演出タイマー。</summary>
	float startCameraTimer_ = 0;
	/// <summary>前回のステート経過時間。</summary>
	float prevStateElapsed_ = 0.0f;

	// JSONマネージャ
	/// <summary>ゲームUI用JSONマネージャ。</summary>
	std::unique_ptr<TYEngine::Utility::JsonManager> gameUIJM_;
	/// <summary>UI設定エラーメッセージ。</summary>
	std::string errUI_;
	/// <summary>パラメータ用JSONマネージャ。</summary>
	std::unique_ptr<TYEngine::Utility::JsonManager> paramJM_;
	/// <summary>パラメータ設定エラーメッセージ。</summary>
	std::string errParam_;
	/// <summary>設定用JSONマネージャ。</summary>
	std::unique_ptr<TYEngine::Utility::JsonManager> configJM_;
	/// <summary>設定エラーメッセージ。</summary>
	std::string errConfig_;


	/// <summary>ステートマシーン。</summary>
	StateMachineType stateMachine_;

};

// --- 状態クラスの定義 ---
class GameSceneStateLoad : public TYEngine::Utility::State<GameSceneState, GameScene>
{
public:
	using State::State;
	void Init(GameScene& owner) override;
	void Update(GameScene& owner, float deltaTime) override;
	void Exit(GameScene& owner) override;
};

class GameSceneStateFadeIn : public TYEngine::Utility::State<GameSceneState, GameScene>
{
public:
	using State::State;
	void Init(GameScene& owner) override;
	void Update(GameScene& owner, float deltaTime) override;
	void Exit(GameScene& owner) override;
};

class GameSceneStateReady : public TYEngine::Utility::State<GameSceneState, GameScene>
{
public:
	using State::State;
	void Init(GameScene& owner) override;
	void Update(GameScene& owner, float deltaTime) override;
	void Exit(GameScene& owner) override;
};

class GameSceneStatePlay : public TYEngine::Utility::State<GameSceneState, GameScene>
{
public:
	using State::State;
	void Init(GameScene& owner) override;
	void Update(GameScene& owner, float deltaTime) override;
	void Exit(GameScene& owner) override;
};

class GameSceneStatePause : public TYEngine::Utility::State<GameSceneState, GameScene>
{
public:
	using State::State;
	void Init(GameScene& owner) override;
	void Update(GameScene& owner, float deltaTime) override;
	void Exit(GameScene& owner) override;
};

class GameSceneStateDead : public TYEngine::Utility::State<GameSceneState, GameScene>
{
public:
	using State::State;
	void Init(GameScene& owner) override;
	void Update(GameScene& owner, float deltaTime) override;
	void Exit(GameScene& owner) override;
};

class GameSceneStateClear : public TYEngine::Utility::State<GameSceneState, GameScene>
{
public:
	using State::State;
	void Init(GameScene& owner) override;
	void Update(GameScene& owner, float deltaTime) override;
	void Exit(GameScene& owner) override;
};

class GameSceneStateResult : public TYEngine::Utility::State<GameSceneState, GameScene>
{
public:
	using State::State;
	void Init(GameScene& owner) override;
	void Update(GameScene& owner, float deltaTime) override;
	void Exit(GameScene& owner) override;
};

class GameSceneStateRetry : public TYEngine::Utility::State<GameSceneState, GameScene>
{
public:
	using State::State;
	void Init(GameScene& owner) override;
	void Update(GameScene& owner, float deltaTime) override;
	void Exit(GameScene& owner) override;
};

class GameSceneStateFadeOut : public TYEngine::Utility::State<GameSceneState, GameScene>
{
public:
	using State::State;
	void Init(GameScene& owner) override;
	void Update(GameScene& owner, float deltaTime) override;
	void Exit(GameScene& owner) override;
};

class GameSceneStateDebugEdit : public TYEngine::Utility::State<GameSceneState, GameScene>
{
public:
	using State::State;
	void Init(GameScene& owner) override;
	void Update(GameScene& owner, float deltaTime) override;
	void Exit(GameScene& owner) override;
};
