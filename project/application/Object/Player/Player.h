#pragma once
#include "../BaseCharacter/BaseCharacter.h"
#include "PlayerStruct.h"
#include "PlayerCollider.h"
#include "JustCollider.h"
#include "./PAttackStrategy/IPAttackStrategy.h"
#include "StateMachine.h"
#include "State.h"
#include "Sprite.h"
#include "BeatAnalyzer.h"
#include "Reticle/Reticle.h"
#include <numbers>

enum class PlayerState
{
	IDLE,
    ROUTE,
    BOOST,
    BARREL_ROLL,
    TAKE_DAMAGE,
    DEAD,
};

namespace TYEngine::Framework
{
	class Input;
}
class Camera;
class EnemyManager;
class Enemy;

// 前方宣言
class PlayerStateIdle;
class PlayerStateRoute;
class PlayerStateBoost;
class PlayerStateBarrelRoll;
class PlayerStateTakeDamage;
class PlayerStateDead;

class Player :
	public BaseCharacter
{
	friend class PlayerStateIdle;
	friend class PlayerStateRoute;
	friend class PlayerStateBoost;
	friend class PlayerStateBarrelRoll;
	friend class PlayerStateTakeDamage;
	friend class PlayerStateDead;

public:
	using StateMachineType = TYEngine::Utility::StateMachine<PlayerState, Player>;

public:
	virtual ~Player();
    /// <summary>
    /// 初期化処理。
    /// 状態マシンの初期化、弾マネージャの生成などを行う。
    /// </summary>
    void Init() override;

	/// <summary>
	/// プレイヤーの状態を初期状態にリセットする。
	/// リトライ時などに使用する。
	/// </summary>
	void Reset();

    /// <summary>
    /// 毎フレーム更新処理。
    /// 入力に応じた移動、攻撃、状態遷移を行う。
    /// </summary>
    void Update() override;

    /// <summary>
    /// 描画処理。
    /// 3Dモデル、弾、レティクルの描画を行う。
    /// </summary>
    void Draw() override;

	/// <summary>
	/// 描画処理。
	/// UIの描画を行う。
	/// </summary>
	void DrawUI();

	/// <summary>
	/// プレイヤーに使用させるカメラを設定する。
	/// </summary>
	void SetCamera(TYEngine::CameraSystem::Camera* camera) { camera_ = camera; }

	/// <summary>
	/// プレイヤーの3Dオブジェクトを取得する。
	/// </summary>
	TYEngine::Graphics::Object3d* GetObj() { return obj_.get(); }

	/// <summary>ジャスト回避成功時のフラグを立てる。</summary>
	void OnJust() { barrelRoll_.isJust = true; }
	/// <summary>ジャスト回避フラグを下ろす。</summary>
	void OffJust() { barrelRoll_.isJust = false; }

	/// <summary>ジャスト回避成功状態かを取得する。</summary>
	bool IsJust() { return barrelRoll_.isJust; }

	/// <summary>リズム判定処理。</summary>
	void RhythmJudgment();

	/// <summary>
	/// 画面内での相対オフセット位置を設定する。
	/// </summary>
	/// <param name="offset">オフセット（-1.0 ~ 1.0）。</param>
	void SetScreenOffset(const TYEngine::Utility::Vector2& offset) { screenOffset_ = offset; }

	/// <summary>
	/// ダメージを受ける処理。
	/// HPを減らし、無敵時間や被弾演出を開始する。
	/// </summary>
	void TakeDamage();

	/// <summary>現在のHPを取得する。</summary>
	int GetHP() { return status_.hitPoint; }

	/// <summary>現在の画面内オフセットを取得する。</summary>
	TYEngine::Utility::Vector2& GetScreenOffset() { return screenOffset_; }

	StateMachineType& GetStateMachine() { return stateMachine_; }

	PlayerLockOn& GetLockOn() { return lockOn_; }

	PlayerBullets& GetBullets() { return  bullets_; }

	PlayerStatus& GetStatus() { return status_; }

	Reticle* GetReticle() { return  reticle_.get(); }

	TYEngine::Framework::Input* GetInput() { return input_; }

	TYEngine::CameraSystem::Camera* GetCamera() { return camera_; }

	TYEngine::AudioSystem::BeatAnalyzer* GetBeatAnalyzer() { return beatAnalyzer_; }

	/// <summary>
	/// 衝突時コールバック。
	/// 敵や敵弾と衝突した際の処理を行う。
	/// </summary>
	void OnCollision() override;

	void SetBGMHandle(int handle) { BGMHandle_ = handle; }

	void SetEnemyManager(EnemyManager* manager) { lockOn_.enemyManager = manager; }

	void SetIsInGame(bool isInGame) { isInGame_ = isInGame; }

	void SetBeatAnalyzer(TYEngine::AudioSystem::BeatAnalyzer* beatAnalyzer) { beatAnalyzer_ = beatAnalyzer; }

	/// <summary>
	/// 攻撃戦略（ストラテジーパターン）を設定する。
	/// </summary>
	/// <param name="strategy">攻撃アルゴリズムのインスタンス。</param>
	void SetAttackStrategy(std::unique_ptr<PlayerAttack::IPAttackStrategy> strategy)
	{
		attackStrategy_ = std::move(strategy);
	}

private:
	/// <summary>状態更新後の共通処理（フラグ管理など）。</summary>
	void PostStateUpdate();

	/// <summary>攻撃処理（弾の発射）。</summary>
	void Attack();
	/// <summary>移動処理（入力に応じた位置更新）。</summary>
	void Move();
	/// <summary>移動範囲の制限（クランプ）。</summary>
	void ClampOffset();

	/// <summary>ローカル回転の更新（機体の傾き）。</summary>
	void RotationOffsetLocal();

	/// <summary>
	/// スクリーン座標オフセットをワールド座標に変換する。
	/// </summary>
	/// <param name="offset">スクリーンオフセット。</param>
	/// <returns>ワール座標。</returns>
	TYEngine::Utility::Vector3 ConvertScreenOffsetToWorld(const TYEngine::Utility::Vector2& offset);

	// BarrelRoll
	/// <summary>バレルロール（回避）開始処理。</summary>
	void StartBarrelRoll();
	/// <summary>バレルロール中の更新処理。</summary>
	void BarrelRoll();
	/// <summary>左方向へのロール。</summary>
	void LeftRoll(const TYEngine::Utility::Vector2& dir);
	/// <summary>右方向へのロール。</summary>
	void RightRoll(const TYEngine::Utility::Vector2& dir);

	/// <summary>パーティクル演出の更新処理。</summary>
	void ParticleUpdate();


	/// <summary>JSONマネージャー関連の初期化。</summary>
	void JMInit();

	// Debug
	/// <summary>デバッグ時に必要な更新処理。</summary>
	void DebugUpdate();
	/// <summary>デバッグ用GUIの表示。</summary>
	void DebugGUI();
	/// <summary>JSONパラメータの反映（デバッグ用）。</summary>
	void DebugJMApply();

private:
	/// <summary>入力管理クラス。</summary>
	TYEngine::Framework::Input* input_ = nullptr;
	/// <summary>使用するカメラ。</summary>
	TYEngine::CameraSystem::Camera* camera_ = nullptr;
	/// <summary>カメラ基準のスクリーン内オフセット（例：[-1, 1]）。</summary>
	TYEngine::Utility::Vector2 screenOffset_{};
	/// <summary>プレイヤー用コライダー。</summary>
	std::unique_ptr<PlayerCollider> collider_;
	/// <summary>ジャスト回避判定用コライダー。</summary>
	std::unique_ptr<JustCollider> justCollider_;

	/// <summary>コライダーのスケール。</summary>
	float colliderScale_ = 0.0f;

	/// <summary>プレイヤーの移動関連。</summary>
	PlayerMovement movement_;
	/// <summary>プレイヤーのステータス。</summary>
	PlayerStatus status_;
	/// <summary>プレイヤーのバレルロール関連。</summary>
	PlayerBarrelRoll barrelRoll_;
	/// <summary>プレイヤーのロックオン関連。</summary>
	PlayerLockOn lockOn_;
	/// <summary>プレイヤーの弾関連。</summary>
	PlayerBullets bullets_;
	/// <summary>レティクル管理クラス。</summary>
	std::unique_ptr<Reticle> reticle_;


	/// <summary>現在の攻撃戦略。</summary>
	std::unique_ptr<PlayerAttack::IPAttackStrategy> attackStrategy_;

	PlayerJetEffect jetEffect_;

	PlayerDestroyEffect destroyEffect_;
	// キリモミ落下時のパラメータ
	PlayerDeadMotion deadMotion_;
	// 被弾時のパラメータ
	PlayerTakeDamageParams takeDamage_;

	/// <summary>BGM再生ハンドル。</summary>
	int BGMHandle_ = -1;

	/// <summary>BeatAnalyzerのポインタ。</summary>
	TYEngine::AudioSystem::BeatAnalyzer* beatAnalyzer_ = nullptr;

	/// <summary>プレイヤーの生成場所がインゲームかどうかのフラグ。</summary>
	bool isInGame_ = false;

	/// <summary>ステートマシーン。</summary>
	StateMachineType stateMachine_;

	/// <summary>JSONマネージャ。</summary>
	TYEngine::Utility::JsonManager jm_;
	/// <summary>JSONエラーメッセージ。</summary>
	std::string err_;

};

// --- 状態クラスの定義 ---
class PlayerStateIdle : public TYEngine::Utility::State<PlayerState, Player>
{
public:
	using State::State;
	void Init(Player& owner) override;
	void Update(Player& owner, float deltaTime) override;
	void Exit(Player& owner) override;
};

class PlayerStateRoute : public TYEngine::Utility::State<PlayerState, Player>
{
public:
	using State::State;
	void Init(Player& owner) override;
	void Update(Player& owner, float deltaTime) override;
	void Exit(Player& owner) override;
};

class PlayerStateBoost : public TYEngine::Utility::State<PlayerState, Player>
{
public:
	using State::State;
	void Init(Player& owner) override;
	void Update(Player& owner, float deltaTime) override;
	void Exit(Player& owner) override;
};

class PlayerStateBarrelRoll : public TYEngine::Utility::State<PlayerState, Player>
{
public:
	using State::State;
	void Init(Player& owner) override;
	void Update(Player& owner, float deltaTime) override;
	void Exit(Player& owner) override;
};

class PlayerStateTakeDamage : public TYEngine::Utility::State<PlayerState, Player>
{
public:
	using State::State;
	void Init(Player& owner) override;
	void Update(Player& owner, float deltaTime) override;
	void Exit(Player& owner) override;
};

class PlayerStateDead : public TYEngine::Utility::State<PlayerState, Player>
{
public:
	using State::State;
	void Init(Player& owner) override;
	void Update(Player& owner, float deltaTime) override;
	void Exit(Player& owner) override;
};

