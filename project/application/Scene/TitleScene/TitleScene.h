#pragma once
#include "IScene.h"
#include "StateMachine.h"
#include "State.h"
#include "ObjectCubemap.h"
#include "Object/Player/Player.h"
#include "../../Object/Enemy/EnemyManager/EnemyManager.h"

#include "AudioAnalyzer.h"
#include "BeatAnalyzer.h"

#define JSONMGR_WITH_IMGUI
#include "Utils/Json/JsonManager.h"

#include "Sprite.h"

#include "Oscillator.h"

class GameAudio;

enum class TitleSceneState
{
	FADE_IN,	// シーン開始時のフェードイン
	READY,		// シーン開始時の演出
	PLAY,		// 通常の進行
	PAUSE,		// 一時停止状態（メニューなど）
	FADE_OUT,	// シーン遷移時のフェードアウト
};

// 前方宣言
class TitleSceneStateFadeIn;
class TitleSceneStateReady;
class TitleSceneStatePlay;
class TitleSceneStatePause;
class TitleSceneStateFadeOut;

/// <summary>
/// タイトル画面シーン。
/// ゲーム開始前の演出、操作説明、ゲーム本編への遷移を管理する。
/// </summary>
class TitleScene : 
	public TYEngine::Framework::IScene 
{
	friend class TitleSceneStateFadeIn;
	friend class TitleSceneStateReady;
	friend class TitleSceneStatePlay;
	friend class TitleSceneStatePause;
	friend class TitleSceneStateFadeOut;

public:
	using StateMachineType = TYEngine::Utility::StateMachine<TitleSceneState, TitleScene>;

public:
	TitleScene();
	~TitleScene();

	/// <summary>
	/// 初期化処理。
	/// タイトルロゴ、背景、デモプレイ用キャラなどの生成を行う。
	/// </summary>
	void Init() override;

	/// <summary>
	/// 更新処理。
	/// 入力受付やシーン遷移制御を行う。
	/// </summary>
	void Update() override;

	/// <summary>
	/// 3Dオブジェクト等の描画。
	/// </summary>
	void Draw() override;

	/// <summary>
	/// UI（スプライト）描画処理。
	/// </summary>
	void UIDraw() override;

private:
	/// <summary>JSONパラメータの反映（デバッグ用）。</summary>
	void DebugJMApply();

	/// <summary>JSONパラメータの反映（デバッグ用）。</summary>
	void DebugUpdate();

	/// <summary>ロードまとめ。</summary>
	void Load();

private:
	/// <summary>スペース（Press Space）表示スプライト。</summary>
	std::unique_ptr<TYEngine::Graphics::Sprite> enterSpr_;
	/// <summary>タイトルテキストスプライト。</summary>
	std::unique_ptr<TYEngine::Graphics::Sprite> text_;
	/// <summary>操作説明スプライト。</summary>
	std::unique_ptr<TYEngine::Graphics::Sprite> operation_;
	/// <summary>レティクルスプライト（演出用）。</summary>
	std::unique_ptr<TYEngine::Graphics::Sprite> reticle_;

	/// <summary>スカイボックス。</summary>
	std::unique_ptr<TYEngine::Graphics::ObjectCubemap> skybox_;

	/// <summary>デモ用プレイヤー。</summary>
	std::unique_ptr<Player> player_;

	/// <summary>デモ用敵マネージャ。</summary>
	EnemyManager enemyMgr_;


	TYEngine::Utility::Oscillator oscillator_;

	/// <summary>背景（地面）。</summary>
	std::unique_ptr<TYEngine::Graphics::Object3d> ground_;
	/// <summary>背景のワールド変換情報。</summary>
	TYEngine::Utility::WorldTransform groundWT_;
	/// <summary>背景の回転速度。</summary>
	float rotateSpeed_ = 0.0f;

	/// <summary>JSONマネージャ（デバッグ設定用）。</summary>
	TYEngine::Utility::JsonManager titleJM;
	/// <summary>JSONエラーメッセージ。</summary>
	std::string err;

	/// <summary>オーディオスペクトラムアナライザー（演出用）。</summary>
	TYEngine::AudioSystem::AudioAnalyzer audioAnalyzer_;

	/// <summary>BGM再生ハンドル。</summary>
	int bgmHandle_ = -1;

	/// <summary>ゲームオーディオ管理クラスのポインタ。</summary>
	GameAudio* gameAudio_ = nullptr;

	/// <summary>ステートマシーン。</summary>
	StateMachineType stateMachine_; 
};

// --- 状態クラスの定義 ---
/// <summary>タイトルシーンのフェードイン状態クラス。</summary>
class TitleSceneStateFadeIn : public TYEngine::Utility::State<TitleSceneState, TitleScene>
{
public:
	using State::State;
	void Init(TitleScene& owner) override;
	void Update(TitleScene& owner, float deltaTime) override;
	void Exit(TitleScene& owner) override;
};

/// <summary>タイトルシーンの開始準備・演出待機状態クラス。</summary>
class TitleSceneStateReady : public TYEngine::Utility::State<TitleSceneState, TitleScene>
{
public:
	using State::State;
	void Init(TitleScene& owner) override;
	void Update(TitleScene& owner, float deltaTime) override;
	void Exit(TitleScene& owner) override;
};

/// <summary>タイトルシーンのメインループ・入力待ち状態クラス。</summary>
class TitleSceneStatePlay : public TYEngine::Utility::State<TitleSceneState, TitleScene>
{
public:
	using State::State;
	void Init(TitleScene& owner) override;
	void Update(TitleScene& owner, float deltaTime) override;
	void Exit(TitleScene& owner) override;
};

/// <summary>タイトルシーンの一時停止状態クラス。</summary>
class TitleSceneStatePause : public TYEngine::Utility::State<TitleSceneState, TitleScene>
{
public:
	using State::State;
	void Init(TitleScene&) override {}
	void Update(TitleScene&, float) override {}
	void Exit(TitleScene&) override {}
};

/// <summary>タイトルシーンのフェードアウト状態クラス。</summary>
class TitleSceneStateFadeOut : public TYEngine::Utility::State<TitleSceneState, TitleScene>
{
public:
	using State::State;
	void Init(TitleScene& owner) override;
	void Update(TitleScene& owner, float deltaTime) override;
	void Exit(TitleScene& owner) override;
};
