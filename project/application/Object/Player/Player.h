#pragma once
#include "../BaseCharacter/BaseCharacter.h"
#include "PlayerCollider.h"
#include "JustCollider.h"
#include "StateMachine.h"
#include "Sprite.h"
#include "PlayerBullet/PlayerBulletType.h"
#include "PlayerBullet/PlayerBulletManager.h"
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

namespace TYEngine
{
	namespace Framework
	{
		class Input;
	}
}

class Camera;
class EnemyManager;
class Enemy;

class Player :
	public BaseCharacter, public TYEngine::Utility::StateMachine<Player, PlayerState>
{
public: // 関数テーブル
    static const std::vector<StateFunctionSet>& GetStateTable();

public:
	virtual ~Player();
    /// <summary>
    /// 初期化処理。
    /// 状態マシンの初期化、弾マネージャの生成などを行う。
    /// </summary>
    void Init() override;

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
	void OnJust() { isJust_ = true; }
	/// <summary>ジャスト回避フラグを下ろす。</summary>
	void OffJust() { isJust_ = false; }

	/// <summary>ジャスト回避成功状態かを取得する。</summary>
	bool IsJust() { return isJust_; }

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
	int GetHP() { return hitPoint_; }

	/// <summary>現在の画面内オフセットを取得する。</summary>
	TYEngine::Utility::Vector2 GetScreenOffset() { return screenOffset_; }

	/// <summary>
	/// 衝突時コールバック。
	/// 敵や敵弾と衝突した際の処理を行う。
	/// </summary>
	void OnCollision() override;

	void SetBGMHandle(int handle) { BGMHandle_ = handle; }

	void SetEnemyManager(EnemyManager* manager) { enemyManager_ = manager; }

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


	// Debug
	/// <summary>デバッグ用GUIの表示。</summary>
	void DebugGUI();

	// Reticle
	/// <summary>レティクル初期化。</summary>
	void ReticleInit();
	/// <summary>レティクル更新。</summary>
	void ReticleUpdate();
	/// <summary>レティクル描画。</summary>
	void ReticleDraw();

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



	EnemyManager* enemyManager_ = nullptr; // 敵マネージャへのアクセス用
	// ロックオン関連
	std::vector<Enemy*> lockedEnemies_;
	int maxLockCount_ = 8;          // 最大ロックオン数
	float lockOnRadius_ = 10.0f;     // ロックオン判定の広さ
	bool wasPressingShot_ = false;  // 前フレームでボタンを押していたか
	const float maxLockOnCool_ = 0.3f;
	float lockOnTimer_ = 0.0f;
	/// <summary>ロックオンスプライト。</summary>
	std::array<std::unique_ptr<TYEngine::Graphics::Sprite>, 8> lockOnSpr_;


	/// <summary>コライダーのスケール。</summary>
	float colliderScale_ = 0.0f;
	/// <summary>カメラからの深度距離。</summary>
	float playerDepthFromCamera_ = 0.0f;
	/// <summary>横移動の最大幅（画面内の物理スケール）。</summary>
	float xRange = 0.0f;
	/// <summary>縦移動の最大高さ。</summary>
	float yRange = 0.0f;

	/// <summary>基本移動速度。</summary>
	float defaultSpeed_ = 0.0f;
	/// <summary>現在の移動速度。</summary>
	TYEngine::Utility::Vector2 speed_{};
	/// <summary>入力方向。</summary>
	TYEngine::Utility::Vector2 inputDir_{};

	/// <summary>ロール回転方向。</summary>
	TYEngine::Utility::Vector2 rollDir_{};

	/// <summary>デルタタイム。</summary>
	float deltaTime_ = 0.0f;

	/// <summary>ヒットポイント。</summary>
	int hitPoint_ = 0;


	// 姿勢
	/// <summary>Yaw：Y軸周りの回転（左右の振り向き）。</summary>
	float yaw = 0.0f;
	/// <summary>Pitch：X軸周りの回転（上下の仰角）。</summary>
	float pitch = 0.0f;
	/// <summary>Roll：Z軸周りの回転（傾き）。</summary>
	float roll = 0.0f;

	/// <summary>移動に伴うPitch傾斜。</summary>
	float movePitch = 0.0f;

	// バレルロール
	/// <summary>ロール所要時間。</summary>
	float rollTime_ = 0.0f;
	/// <summary>ロール移動範囲。</summary>
	float rollRange_ = 0.0f;
	/// <summary>左ロール目標角度。</summary>
	float leftRoll_ = 0.0f;
	/// <summary>右ロール目標角度。</summary>
	float rightRoll_ = 0.0f;
	/// <summary>ロール開始位置。</summary>
	TYEngine::Utility::Vector2 startRollPos_{};
	/// <summary>ロール終了位置。</summary>
	TYEngine::Utility::Vector2 goalRollPos_{};

	/// <summary>ロールエフェクトタイマー。</summary>
	float rollEffectTimer_ = 0.0f;

	/// <summary>ジャスト回避成功フラグ。</summary>
	bool isJust_ = false;
	/// <summary>ジャスト回避ロール中かどうか。</summary>
	bool justRoll_ = false;
	/// <summary>ジャスト回避時のスケール倍率。</summary>
	float justScale_ = 0.0f;


	// 弾関連
	/// <summary>弾管理マネージャ。</summary>
	std::unique_ptr<PlayerBulletManager> bulletManager_;
	/// <summary>現在の弾タイプ。</summary>
	PlayerBulletType currentBulletType_ = PlayerBulletType::NORMAL;
	/// <summary>発射クールタイム。</summary>
	float bulletCoolTime_ = 0.0f;
	/// <summary>発射タイマー。</summary>
	float bulletTimer_ = 0.0f;

	/// <summary>レティクル管理クラス。</summary>
	std::unique_ptr<Reticle> reticle_;


	/// <summary>デバッグ用レティクルオブジェクト。</summary>
	std::unique_ptr<TYEngine::Graphics::Object3d> reticleObj_;
	/// <summary>レティクルワールド変換。</summary>
	TYEngine::Utility::WorldTransform reticleWT_;

	/// <summary>コントレイルインデックス。</summary>
	int contrailIndex_;

	/// <summary>BGM再生ハンドル。</summary>
	int BGMHandle_ = -1;


private: // シーン内のState関連関数
#pragma region // State関連関数
	// 列挙名を文字列化（ImGui表示用）
	std::string GetStateName(State state) const override 
	{
		switch (state) 
		{
		case State::IDLE: return "IDLE";
		case State::ROUTE: return "ROUTE";
		case State::BOOST: return "BOOST";
		case State::BARREL_ROLL: return "BARREL_ROLL";
		case State::TAKE_DAMAGE: return "TAKE_DAMAGE";
		case State::DEAD: return "DEAD";
		default: return "Unknown";
		}
	}

	// 待機状態
	void InitIdle();
	void UpdateIdle();
	void ExitIdle();

	// 通常行動
	void InitRoute();
	void UpdateRoute();
	void ExitRoute();

	// 加速
	void InitBoost();
	void UpdateBoost();
	void ExitBoost();

	// 回避
	void InitBarrelRoll();
	void UpdateBarrelRoll();
	void ExitBarrelRoll();

	// 被弾
	void InitTakeDamage();
	void UpdateTakeDamage();
	void ExitTakeDamage();

	// 死亡
	void InitDead();
	void UpdateDead();
	void ExitDead();
#pragma endregion
};

