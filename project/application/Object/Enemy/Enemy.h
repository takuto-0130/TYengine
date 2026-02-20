#pragma once
#include "../BaseCharacter/BaseCharacter.h"
#include "EnemyCollider.h"
#include "StateMachine.h"
#include "Ease.h"
#include "IParticleRenderer.h"
#include "../../AppSystem/EventListener/EnemyEvent/IEnemyEventListener.h"
#include "AttackStrategy/IAttackStrategy.h"
#include <iostream>
#include <random>

class EnemyBulletManager;

/// <summary>
/// 敵の状態（ステート）定義。
/// </summary>
enum class EnemyState
{
	PRE_ENTER,	// 画面外・未表示（NDCの外）
	ENTERING,	// 画面外→画面内へ（イージングで移動）
	ACTIVE,		// 敵アクティブ（攻撃・移動など）
	EXITING,	// 画面内→画面外へ（退場）
	DAMAGED,	// 被弾（ダメージ演出）
	DESPAWNED,	// 破棄（スコア加算などの処理を挟んでから消滅）
};

/// <summary>
/// 敵のタイプ。
/// </summary>
enum EnemyType
{
	Normal,
	Vertical,
	Horizontal,
	Triangle
};

/// <summary>
/// 敵キャラクターのクラス。
/// 状態マシンにより行動を制御し、出現・攻撃・被弾・死亡などのライフサイクルを管理する。
/// </summary>
class Enemy :
    public BaseCharacter
{
public:
	using StateMachineType = TYEngine::Utility::StateMachine<Enemy, EnemyState>;
	using StateFunctionSet = StateMachineType::StateFunctionSet;

public: // 関数テーブル
	static const std::vector<StateFunctionSet>& GetStateTable();

public:
	~Enemy() override;

	/// <summary>
	/// 初期化処理。
	/// コライダー生成、ステート初期化、パーティクル設定などを行う。
	/// </summary>
	void Init() override;

	/// <summary>
	/// 毎フレームの更新処理。
	/// 状態更新、移動、攻撃タイマー更新などを行う。
	/// </summary>
	void Update() override;

	/// <summary>
	/// 行列（ワールドトランスフォーム）の更新。
	/// </summary>
	void UpdateTransform();

	/// <summary>
	/// 描画処理。
	/// 3Dモデルの描画を行う。
	/// </summary>
	void Draw() override;

	/// <summary>
	/// 敵を出現させる（ポップアニメーション開始）。
	/// </summary>
	void Pop();

	/// <summary>
	/// 衝突時コールバック。
	/// プレイヤー弾との判定処理など。
	/// </summary>
	void OnCollision() override;

public:
	/// <summary>
	/// 敵イベントリスナー（撃破時の通知先など）を設定する。
	/// </summary>
	void SetEventListener(IEnemyEventListener* listener) { listener_ = listener; }

	/// <summary>
	/// 敵弾マネージャを設定する（弾発射に使用）。
	/// </summary>
	void SetEnemyBulletManager(EnemyBulletManager* bulletManager) { bulletManager_ = bulletManager; }

	/// <summary>
	/// 座標を強制的に設定し、行列更新を行う。
	/// </summary>
	/// <param name="pos">設定座標。</param>
	void SetAndApplyPos(const TYEngine::Utility::Vector3& pos)
	{
		worldTransform_.SetTranslation(pos);
		worldTransform_.Update();
	}

	/// <summary>
	/// 移動目標座標を設定する。
	/// </summary>
	void SetTargetPos(const TYEngine::Utility::Vector3& pos) { targetPos_ = pos; }

	/// <summary>
	/// ゲーム中（アクティブ）かどうかのフラグを設定する。
	/// </summary>
	void SetIsInGame(bool is) { isInGame_ = is; }

	/// <summary>
	/// 参照用カメラを設定する。
	/// </summary>
	void SetCamera(TYEngine::CameraSystem::Camera* camera) { camera_ = camera; }

	/// <summary>
	/// スクリーン座標（デバッグ用・配置ツール用）を設定する。
	/// </summary>
	void SetScreenPos(const TYEngine::Utility::Vector2& pos) { screenPos = pos; }

	/// <summary>
	/// スクリーン座標を取得する。
	/// </summary>
	const TYEngine::Utility::Vector2& GetScreenPos() { return screenPos; }

	/// <summary>
	/// 攻撃戦略（ストラテジーパターン）を設定する。
	/// </summary>
	/// <param name="strategy">攻撃アルゴリズムのインスタンス。</param>
	void SetAttackStrategy(std::unique_ptr<EnemyAttack::IAttackStrategy> strategy)
	{
		attackStrategy_ = std::move(strategy);
	}

	const TYEngine::Utility::Vector3& GetDefaultScale() { return defaultScale_; }
	const TYEngine::Utility::Vector3& GetUpScale() { return upScale_; }
	StateMachineType GetStateMachine() { return stateMachine_; }

private:
	/// <summary>
	/// 射撃処理を実行する。
	/// </summary>
	void IsShot();

	/// <summary>
	/// 回転挙動の更新。
	/// </summary>
	void Rotate();

private:
	/// <summary>敵用コライダー（当たり判定）。</summary>
	std::unique_ptr<EnemyCollider> collider_;

	/// <summary>パーティクルエミッター。</summary>
	TYEngine::Effect::IParticleRenderer::Emitter emitter;

	/// <summary>出現演出にかかる時間（秒）。</summary>
	float popTime_ = 0.0f;

	/// <summary>敵の種類ID。</summary>
	int enemyType_ = 0;

	/// <summary>現在の攻撃戦略。</summary>
	std::unique_ptr<EnemyAttack::IAttackStrategy> attackStrategy_;


	/// <summary>弾発射のクールタイム（秒）。</summary>
	float bulletCoolTime_ = 0.0f;
	/// <summary>弾発射用タイマー。</summary>
	float bulletTimer_ = 0.0f;

	/// <summary>通常スケール。</summary>
	TYEngine::Utility::Vector3 defaultScale_ = { 1, 1, 1 };
	/// <summary>拡大スケール（演出用）。</summary>
	TYEngine::Utility::Vector3 upScale_ = { 1, 1, 1 };
	/// <summary>ゼロスケール（出現前）。</summary>
	const TYEngine::Utility::Vector3 ZeroScale = {};

	/// <summary>移動目標地点。</summary>
	TYEngine::Utility::Vector3 targetPos_ = {};

	/// <summary>画面上の座標（エディタ用）。</summary>
	TYEngine::Utility::Vector2 screenPos = {};

	// 発射予兆角度
	/// <summary>発射予兆 Yaw角。</summary>
	float shotYaw_ = 0;
	/// <summary>発射予兆 Pitch角。</summary>
	float shotPitch_ = 0;
	/// <summary>発射予兆 Roll角。</summary>
	float shotRoll_ = 0;


	/// <summary>イベントリスナー。</summary>
	IEnemyEventListener* listener_ = nullptr;
	/// <summary>敵弾マネージャへのポインタ。</summary>
	EnemyBulletManager* bulletManager_;

	/// <summary>乱数生成器。</summary>
	std::random_device rd;

	/// <summary>ゲームプレイ中フラグ（一時停止や演出中は false）。</summary>
	bool isInGame_ = true;

	/// <summary>寿命（秒）。</summary>
	float lifeTime_ = 0.0f;

	/// <summary>体力。</summary>
	int32_t hitPoint_ = 0;

	/// <summary>ロール回転角度。</summary>
	float roll_ = 0.0f;

	/// <summary>カメラ。</summary>
	TYEngine::CameraSystem::Camera* camera_ = nullptr;

	/// <summary>ステートマシーン。</summary>
	StateMachineType stateMachine_;


private: // シーン内のState関連関数
#pragma region // State関連関数
	// 待機状態
	void InitPreEnter() {};
	void UpdatePreEnter() {};
	void ExitPreEnter() {};

	// スポーン
	void InitEntering() {};
	void UpdateEntering();
	void ExitEntering() { worldTransform_.SetScale(defaultScale_); }

	// 通常行動
	void InitActive() {}
	void UpdateActive();
	void ExitActive() {}

	// 退場演出
	void InitExiting() {}
	void UpdateExiting() {}
	void ExitExiting() {}

	// 被弾
	void InitDamaged();
	void UpdateDamaged();
	void ExitDamaged();

	// 死亡
	void InitDespawned();
	void UpdateDespawned();
	void ExitDespawned();
#pragma endregion
};
