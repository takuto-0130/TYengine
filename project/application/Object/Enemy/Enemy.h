#pragma once
#include "../BaseCharacter/BaseCharacter.h"
#include "EnemyCollider.h"
#include "StateMachine.h"
#include "Ease.h"
#include "IParticleRenderer.h"
#include "../../AppSystem/EventListener/EnemyEvent/IEnemyEventListener .h"
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
/// 敵キャラクターのクラス。
/// 状態マシンにより行動を制御し、出現・攻撃・被弾・死亡などのライフサイクルを管理する。
/// </summary>
class Enemy :
    public BaseCharacter, public StateMachine<Enemy, EnemyState>
{
public: // 関数テーブル
	static const std::vector<StateFunctionSet>& GetStateTable();

public:
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
	void SetAndApplyPos(const Vector3& pos) 
	{
		worldTransform_.SetTranslation(pos);
		worldTransform_.Update();
	}

	/// <summary>
	/// 移動目標座標を設定する。
	/// </summary>
	void SetTargetPos(const Vector3& pos) { targetPos_ = pos; }

	/// <summary>
	/// ゲーム中（アクティブ）かどうかのフラグを設定する。
	/// </summary>
	void SetIsInGame(bool is) { isInGame_ = is; }

	/// <summary>
	/// 参照用カメラを設定する。
	/// </summary>
	void SetCamera(Camera* camera) { camera_ = camera; }

	/// <summary>
	/// スクリーン座標（デバッグ用・配置ツール用）を設定する。
	/// </summary>
	void SetScreenPos(const Vector2& pos) { screenPos = pos; }

	/// <summary>
	/// スクリーン座標を取得する。
	/// </summary>
	const Vector2& GetScreenPos() { return screenPos; }

	/// <summary>
	/// 攻撃戦略（ストラテジーパターン）を設定する。
	/// </summary>
	/// <param name="strategy">攻撃アルゴリズムのインスタンス。</param>
	void SetAttackStrategy(std::unique_ptr<EnemyAttack::IAttackStrategy> strategy)
	{
		attackStrategy_ = std::move(strategy);
	}

private:
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
	IParticleRenderer::Emitter emitter;

	/// <summary>出現演出にかかる時間（秒）。</summary>
	const float kPopTime_ = 1.0f;

	/// <summary>敵の種類ID。</summary>
	int enemyType_ = 0;

	/// <summary>現在の攻撃戦略。</summary>
	std::unique_ptr<EnemyAttack::IAttackStrategy> attackStrategy_;


	/// <summary>弾発射のクールタイム（秒）。</summary>
	float kBulletCoolTime_ = 2.0f;
	/// <summary>弾発射用タイマー。</summary>
	float bulletTimer_ = 0.0f;


	/// <summary>1フレームの時間。</summary>
	float deltaTime_ = 1.0f / 60.0f;

	/// <summary>通常スケール。</summary>
	Vector3 defaultScale_ = { 0.3f, 0.3f, 0.3f };
	/// <summary>拡大スケール（演出用）。</summary>
	Vector3 upScale_ = { 0.45f, 0.45f, 0.45f };
	/// <summary>ゼロスケール（出現前）。</summary>
	const Vector3 ZeroScale = {};

	/// <summary>移動目標地点。</summary>
	Vector3 targetPos_ = {};

	/// <summary>画面上の座標（エディタ用）。</summary>
	Vector2 screenPos = {};

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
	float lifeTime_ = 4.0f;

	/// <summary>体力。</summary>
	int32_t hitpoint_ = 3;

	/// <summary>ロール回転角度。</summary>
	float roll_ = 0.0f;

	/// <summary>カメラ。</summary>
	Camera* camera_ = nullptr;


private: // シーン内のState関連関数
#pragma region // State関連関数
	// 列挙名を文字列化（ImGui表示用）
	std::string GetStateName(State state) const override
	{
		switch (state)
		{
		case State::PRE_ENTER: return "PRE_ENTER";
		case State::ENTERING: return "ENTERING";
		case State::ACTIVE: return "ACTIVE";
		case State::EXITING: return "EXITING";
		case State::DAMAGED: return "DAMAGED";
		case State::DESPAWNED: return "DESPAWNED";
		default: return "Unknown";
		}
	}

	// 待機状態
	void InitPreEnter() {};
	void UpdatePreEnter() {};
	void ExitPreEnter() {};

	// スポーン
	void InitEntering() {};
	void UpdateEntering() 
	{
		float t = GetStateElapsedTime() / kPopTime_;
		if (t <= 1.0f)
		{
			worldTransform_.SetScale(Lerp(ZeroScale, defaultScale_, EaseFixed::InOutBounce(t)));
		}
		else
		{
			ChangeState(EnemyState::ACTIVE);
		}
	};
	void ExitEntering() { worldTransform_.SetScale(defaultScale_); }

	// 通常行動
	void InitActive() {}
	void UpdateActive() 
	{
		if (bulletTimer_ > 0.0f)
		{
			bulletTimer_ -= deltaTime_;
			if (bulletTimer_ >= 1.5f)
			{
				float t = bulletTimer_;
				if (t < 1.5f)
				{
					t = 1.5f;
				}
				worldTransform_.SetScale(Lerp(defaultScale_, upScale_, EaseFixed::InOutBounce(t - 1.5f)));
			}

			if (bulletTimer_ <= 0.5f)
			{
				float t = 1.0f - (bulletTimer_ / 0.5f);
				if (enemyType_ == 1)
				{
					// 垂直2点
					shotPitch_ = Lerp(0.0f, 2.0f * std::numbers::pi_v<float>, EaseFixed::InBack(t));
				}
				else if (enemyType_ == 2)
				{
					// 水平4点
					shotYaw_ = Lerp(0.0f, 2.0f * std::numbers::pi_v<float>, EaseFixed::InBack(t));
				}
				else if (enemyType_ == 3)
				{
					// 3角形
					shotRoll_ = Lerp(0.0f, 2.0f * std::numbers::pi_v<float>, EaseFixed::InBack(t));
				}
			}
		}
		else if (bulletTimer_ <= 0.0f)
		{
			shotYaw_ = 0;
			shotPitch_ = 0;
			shotRoll_ = 0;
			IsShot();
		}
	}
	void ExitActive() {}

	// 退場演出
	void InitExiting() {}
	void UpdateExiting() {}
	void ExitExiting() {}

	// 被弾
	void InitDamaged() 
	{
		obj_->SetAddColor({ 1,1,1,1 });
		roll_ = 0.1f;
	}
	void UpdateDamaged() 
	{
		if(GetStateElapsedTime() > 0.05f)
		ChangeState(EnemyState::ACTIVE);
	}
	void ExitDamaged() 
	{
		obj_->SetAddColor({ 0,0,0,0 });
		roll_ = 0.0f;
	}

	// 死亡
	void InitDespawned();
	void UpdateDespawned() 
	{
		if (GetStateElapsedTime() < 2.0f)
		{
			roll_ += 0.02f;
			Vector3 pos = worldTransform_.GetTranslation();
			pos.y -= 0.02f;
			worldTransform_.SetTranslation(pos);
			float t = 1.0f - (GetStateElapsedTime() / 2.0f);
			obj_->SetAlpha(t / 2.0f);
			worldTransform_.SetScale(defaultScale_ * t);
		}
		else
		{
			ChangeState(EnemyState::EXITING);
		}
	}
	void ExitDespawned()
	{
		isDead_ = true;
	}
#pragma endregion
};
