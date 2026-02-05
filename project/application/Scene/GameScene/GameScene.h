#pragma once
#include "IScene.h"
#include "StateMachine.h"

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

class PauseClass;
class ResultClass;
class PlayUI;
class ScoreUI;
class StartUI;
class RetryUI;
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

/// <summary>
/// ゲームシーン
/// </summary>
class GameScene : 
	public TYEngine::Framework::IScene, public TYEngine::Utility::StateMachine<GameScene, GameSceneState>
{
public: // 関数テーブル
	static const std::vector<StateFunctionSet>& GetStateTable();

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

private: // メンバ変数

	/// <summary>カメラのピッチ角。</summary>
	float pitch_ = 1.0f;
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

	
	// UIクラス群
	/// <summary>プレイ中UI。</summary>
	std::unique_ptr<PlayUI> playUI_;
	/// <summary>スタート画面UI。</summary>
	std::unique_ptr<StartUI> startDraw_;
	/// <summary>スコア表示UI。</summary>
	std::unique_ptr<ScoreUI> scoreDraw_;
	/// <summary>リトライ画面UI。</summary>
	std::unique_ptr<RetryUI> retryDraw_;
	/// <summary>ポーズメニュー。</summary>
	std::unique_ptr<PauseClass> pauseMenu_;
	/// <summary>リザルト画面。</summary>
	std::unique_ptr<ResultClass> resultMenu_;

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


	/// <summary>ゲームUI用JSONマネージャ。</summary>
	std::unique_ptr<TYEngine::Utility::JsonManager> gameUIJM_;
	/// <summary>UI設定エラーメッセージ。</summary>
	std::string errUI_;
	/// <summary>設定用JSONマネージャ。</summary>
	std::unique_ptr<TYEngine::Utility::JsonManager> configJM_;
	/// <summary>設定エラーメッセージ。</summary>
	std::string errConfig_;

private: // シーン内のState関連関数
#pragma region // State関連関数
	// 列挙名を文字列化（ImGui表示用）
	std::string GetStateName(State state) const override 
	{
		switch (state) 
		{
		case State::LOAD: return "LOAD";
		case State::FADE_IN: return "FADE_IN";
		case State::READY: return "READY";
		case State::PLAY: return "PLAY";
		case State::PAUSE: return "PAUSE";
		case State::DEAD: return "DEAD";
		case State::CLEAR: return "CLEAR";
		case State::RESULT: return "RESULT";
		case State::RETRY: return "RETRY";
		case State::FADE_OUT: return "FADE_OUT";
		case State::DEBUG_EDIT: return "DEBUG_EDIT";
		default: return "Unknown";
		}
	}

	// ロード
	void InitLoad();
	void UpdateLoad();
	void ExitLoad();

	// フェードイン
	void InitFadeIn();
	void UpdateFadeIn();
	void ExitFadeIn();

	// ゲーム開始直前の演出
	void InitReady();
	void UpdateReady();
	void ExitReady();

	// 通常のゲーム進行
	void InitPlay();
	void UpdatePlay();
	void ExitPlay();

	// 一時停止状態（メニューなど）
	void InitPause();
	void UpdatePause();
	void ExitPause();

	// プレイヤーが死んだ時
	void InitDead();
	void UpdateDead();
	void ExitDead();

	// クリア演出時
	void InitClear();
	void UpdateClear();
	void ExitClear();

	// リザルト
	void InitResult();
	void UpdateResult();
	void ExitResult();

	// リトライ待機中
	void InitRetry();
	void UpdateRetry();
	void ExitRetry();

	// フェードアウト
	void InitFadeOut();
	void UpdateFadeOut();
	void ExitFadeOut();

	// エディタ専用
	void InitDebugEdit();
	void UpdateDebugEdit();
	void ExitDebugEdit();
#pragma endregion
};
