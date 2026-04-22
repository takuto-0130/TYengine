#pragma once
#define JSONMGR_WITH_IMGUI
#include "Utils/Json/JsonManager.h"

#include "Struct.h"
#include "Sprite.h"
#include "PlayerBullet/PlayerBulletType.h"
#include "PlayerBullet/PlayerBulletManager.h"
#include <memory>


/// <summary>プレイヤーの移動関連。</summary>
struct PlayerMovement
{
	/// <summary>カメラからの深度距離。</summary>
	float playerDepthFromCamera = 0.0f;
	/// <summary>横移動の最大幅（画面内の物理スケール）。</summary>
	float xRange = 0.0f;
	/// <summary>縦移動の最大高さ。</summary>
	float yRange = 0.0f;
	/// <summary>基本移動速度。</summary>
	float baseSpeed = 0.0f;
	/// <summary>縦横移動速度。</summary>
	TYEngine::Utility::Vector2 speed{};
	/// <summary>現在の速度計算。</summary>
	void CalculateSpeed() { speed = { baseSpeed, baseSpeed * (xRange / yRange) }; }
	/// <summary>入力方向。</summary>
	TYEngine::Utility::Vector2 inputDir{};
	/// <summary>ロール回転方向。</summary>
	TYEngine::Utility::Vector2 rollDir{};

	// 姿勢
	/// <summary>Yaw：Y軸周りの回転（左右の振り向き）。</summary>
	float yaw = 0.0f;
	/// <summary>Pitch：X軸周りの回転（上下の仰角）。</summary>
	float pitch = 0.0f;
	/// <summary>Roll：Z軸周りの回転（傾き）。</summary>
	float roll = 0.0f;
	/// <summary>移動に伴うPitch傾斜。</summary>
	float movePitch = 0.0f;

	float maxPitch = 0.0f;
	float maxRoll = 0.0f;

	void Load(const TYEngine::Utility::JsonManager& jm)
	{
		playerDepthFromCamera = jm.Get<float>("movement.playerDepthFromCamera");
		xRange = jm.Get<float>("movement.xRange");
		yRange = jm.Get<float>("movement.yRange");
		baseSpeed = jm.Get<float>("movement.baseSpeed");
		maxPitch = jm.Get<float>("movement.maxPitch");
		maxRoll = jm.Get<float>("movement.maxRoll");
		// ロード時に一緒に計算する
		CalculateSpeed();
	}
};

enum HitJudgment
{
	Perfect,	// 最高判定
	Good,		// 普通
	Miss,		// 失敗
	None		// 判定なし
};

/// <summary>プレイヤーのステータス。</summary>
struct PlayerStatus
{
	/// <summary>ヒットポイント最大値。</summary>
	int maxHitPoint = 0;
	/// <summary>ヒットポイント。</summary>
	int hitPoint = 0;
	/// <summary>ヒットポイントのリセット。</summary>
	void HPReset() { hitPoint = maxHitPoint; }
	/// <summary>ヒットポイントバー背景画像。</summary>
	std::unique_ptr<TYEngine::Graphics::Sprite> hpSprBG;
	/// <summary>ヒットポイントバー画像。</summary>
	std::unique_ptr<TYEngine::Graphics::Sprite> hpSpr;

	/// <summary>タイミング良く押せているか。</summary>
	HitJudgment currentJudgment = None;

	/// <summary>ヒットポイントの残量を取得。（ 0 ~1 ）</summary>
	float HPPerf() { return static_cast<float>(hitPoint) / static_cast<float>(maxHitPoint); }

	void Load(const TYEngine::Utility::JsonManager& jm)
	{
		maxHitPoint = jm.Get<int>("status.maxHitPoint");
		// ロード時に一緒に計算する
		HPReset();

		hpSpr = std::make_unique<TYEngine::Graphics::Sprite>();
		hpSpr->Initialize("Resources/Texture/white2x2.png");
		hpSpr->SetPosition(jm.Get<TYEngine::Utility::Vector2>("status.texture.Position"));
		hpSpr->SetSize(jm.Get<TYEngine::Utility::Vector2>("status.texture.Size"));
		hpSpr->SetColor(jm.Get<TYEngine::Utility::Vector4>("status.texture.hpSprColor"));

		hpSprBG = std::make_unique<TYEngine::Graphics::Sprite>();
		hpSprBG->Initialize("Resources/Texture/white2x2.png");
		hpSprBG->SetPosition(jm.Get<TYEngine::Utility::Vector2>("status.texture.Position"));
		hpSprBG->SetSize(jm.Get<TYEngine::Utility::Vector2>("status.texture.Size"));
		hpSprBG->SetColor(jm.Get<TYEngine::Utility::Vector4>("status.texture.hpSprBGColor"));
	}
};

/// <summary>プレイヤーのバレルロール関連。</summary>
struct PlayerBarrelRoll
{
	// バレルロール
	/// <summary>ロール所要時間。</summary>
	float rollTime = 0.0f;
	/// <summary>ロール移動範囲。</summary>
	float rollRange = 0.0f;
	/// <summary>左ロール目標角度。</summary>
	float leftRoll = 0.0f;
	/// <summary>右ロール目標角度。</summary>
	float rightRoll = 0.0f;
	/// <summary>ロール開始位置。</summary>
	TYEngine::Utility::Vector2 startRollPos{};
	/// <summary>ロール終了位置。</summary>
	TYEngine::Utility::Vector2 goalRollPos{};
	/// <summary>ロールエフェクトタイマー。</summary>
	float rollEffectTimer = 0.0f;

	float wholeEffectTime = 0.0f;
	float firstHalfTime = 0.0f;
	float secondHalfTime = 0.0f;
	float blurWidth = 0.0f;
	int blurSamples = 0;

	float audioPitch = 0.0f;

	float slowScale = 0.0f;
	float enterDur = 0.0f;
	float holdDur = 0.0f;
	float exitDur = 0.0f;

	/// <summary>ジャスト回避成功フラグ。</summary>
	bool isJust = false;
	/// <summary>ジャスト回避ロール中かどうか。</summary>
	bool justRoll = false;
	/// <summary>ジャスト回避時のスケール倍率。</summary>
	float justScale = 0.0f;

	void Load(const TYEngine::Utility::JsonManager& jm)
	{
		rollTime = jm.Get<float>("barrelRoll.rollTime");
		rollRange = jm.Get<float>("barrelRoll.rollRange");
		leftRoll = jm.Get<float>("barrelRoll.leftRoll");
		rightRoll = jm.Get<float>("barrelRoll.rightRoll");
		justScale = jm.Get<float>("barrelRoll.justScale");
		wholeEffectTime = jm.Get<float>("barrelRoll.wholeEffectTime");
		firstHalfTime = jm.Get<float>("barrelRoll.firstHalfTime");
		blurWidth = jm.Get<float>("barrelRoll.blurWidth");
		blurSamples = jm.Get<int>("barrelRoll.blurSamples");
		audioPitch = jm.Get<float>("barrelRoll.audioPitch");

		slowScale = jm.Get<float>("barrelRoll.BulletTime.slowScale");
		enterDur = jm.Get<float>("barrelRoll.BulletTime.enterDur");
		holdDur = jm.Get<float>("barrelRoll.BulletTime.holdDur");
		exitDur = jm.Get<float>("barrelRoll.BulletTime.exitDur");

		secondHalfTime = wholeEffectTime - firstHalfTime;
	}

	// タイムラインによるエフェクト強度の制御（前半フェードイン、後半フェードアウト）
	float GetTimelineT()
	{
		return (rollEffectTimer <= firstHalfTime)
			? (rollEffectTimer / firstHalfTime)
			: (secondHalfTime - (rollEffectTimer - firstHalfTime) / secondHalfTime);
	}
	// 通常ロール演出
	float GetRollT()
	{
		return (rollEffectTimer <= secondHalfTime)
			? (rollEffectTimer / secondHalfTime)
			: (secondHalfTime - (rollEffectTimer - secondHalfTime) / secondHalfTime);
	}
};

// 戦法宣言
class EnemyManager;
class Enemy;
/// <summary>プレイヤーのロックオン関連。</summary>
struct PlayerLockOn
{
	EnemyManager* enemyManager = nullptr; // 敵マネージャへのアクセス用
	std::vector<Enemy*> lockedEnemies;
	int maxLockCount = 0;          // 最大ロックオン数
	float lockOnRadius = 0.0f;     // ロックオン判定の広さ
	bool wasPressingShot = false;  // 前フレームでボタンを押していたか
	float maxLockOnCool = 0.0f;
	float lockOnTimer = 0.0f;
	float spreadX = 0.0f;
	float spreadY = 0.0f;
	/// <summary>ロックオンスプライト。</summary>
	std::vector<std::unique_ptr<TYEngine::Graphics::Sprite>> lockOnSpr;

	void Load(const TYEngine::Utility::JsonManager& jm, const std::string& lockOnSprPath = "Resources/Texture/reticle.png")
	{
		maxLockCount = jm.Get<int>("lockOn.maxLockCount");
		lockOnRadius = jm.Get<float>("lockOn.lockOnRadius");
		maxLockOnCool = jm.Get<float>("lockOn.maxLockOnCool");
		spreadX = jm.Get<float>("lockOn.spreadX");
		spreadY = jm.Get<float>("lockOn.spreadY");
		lockOnSpr.resize(maxLockCount);
		for (auto&& spr : lockOnSpr)
		{
			spr = std::make_unique<TYEngine::Graphics::Sprite>();
			spr->Initialize(lockOnSprPath);
			spr->SetAnchorPoint(jm.Get<TYEngine::Utility::Vector2>("lockOn.texture.AnchorPoint"));
			spr->SetColor(jm.Get<TYEngine::Utility::Vector4>("lockOn.texture.Color"));
		}
	}
};

/// <summary>プレイヤーの弾関連。</summary>
struct PlayerBullets
{
	// 弾関連
	/// <summary>弾管理マネージャ。</summary>
	std::unique_ptr<PlayerBulletManager> bulletManager;
	/// <summary>現在の弾タイプ。</summary>
	PlayerBulletType currentBulletType = PlayerBulletType::NORMAL;
	/// <summary>発射クールタイム。</summary>
	float bulletCoolTime = 0.0f;
	/// <summary>発射タイマー。</summary>
	float bulletTimer = 0.0f;

	/// <summary>発射タイマー。</summary>
	float perfectShotThreshold = 0.0f;
	/// <summary>発射タイマー。</summary>
	float goodShotThreshold = 0.0f;


	void Load(const TYEngine::Utility::JsonManager& jm)
	{
		bulletCoolTime = jm.Get<float>("bullets.bulletCoolTime");
		perfectShotThreshold = jm.Get<float>("bullets.perfectShotThreshold");
		goodShotThreshold = jm.Get<float>("bullets.goodShotThreshold");
	}
};

struct PlayerJetEffect
{
	int count = 0;
	float offSet = 0.0f;
	float speed = 0.0f;
	float frequency = 0.0f;
	TYEngine::Utility::Vector3 scale = { 0.0f,0.0f,0.0f };

	void Load(const TYEngine::Utility::JsonManager& jm)
	{
		count = jm.Get<int>("jetEffect.count");
		offSet = jm.Get<float>("jetEffect.offSet");
		speed = jm.Get<float>("jetEffect.speed");
		frequency = jm.Get<float>("jetEffect.frequency");
		scale = jm.Get<TYEngine::Utility::Vector3>("jetEffect.scale");
	}
};

struct PlayerDestroyEffect
{
	int count = 0;
	float frequency = 0.0f;
	TYEngine::Utility::Vector3 scale = { 0.0f, 0.0f, 0.0f };

	void Load(const TYEngine::Utility::JsonManager& jm)
	{
		count = jm.Get<int>("destroyEffect.count");
		frequency = jm.Get<float>("destroyEffect.frequency");
		scale = jm.Get<TYEngine::Utility::Vector3>("destroyEffect.scale");
	}
};

// キリモミ落下時のパラメータ
struct PlayerDeadMotion
{
	float fallSpeedY = -0.5f;   // 落下速度（最初は少し上に跳ねさせるためマイナス値）
	float gravity = 3.0f;       // 重力加速度
	float spinSpeed = 7.0f;    // キリモミの回転速度
	float targetPitch = 1.0f;   // 機体を下に傾ける目標角度（ラジアン）
	float depthSpeed = 0.0f;    // 画面奥へ遠ざかる速度
};
