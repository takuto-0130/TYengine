#include "Enemy.h"
#include "./EnemyBullet/EnemyBulletManager.h"
#include "AttackStrategy/AttackStrategies.h"
#include "Timer.h"
#include "Ease.h"
#include "ColliderManager.h"
#include "ParticleManager.h"
#include "EnemyBullet/Liner/Liner.h"
#include "Random.h"
#include "../../AppSystem/Audio/GameAudio.h"

#define ENEMY_STATE_ENTRY(stateEnum, funcName) \
    STATE_ENTRY_FOR(Enemy, stateEnum, funcName)

const std::vector<StateMachine<Enemy, EnemyState>::StateFunctionSet>& Enemy::GetStateTable()
{
	using enum EnemyState;
	static const std::vector<StateFunctionSet> stateTable =
	{
		ENEMY_STATE_ENTRY(PRE_ENTER, PreEnter),
		ENEMY_STATE_ENTRY(ENTERING, Entering),
		ENEMY_STATE_ENTRY(ACTIVE, Active),
		ENEMY_STATE_ENTRY(EXITING, Exiting),
		ENEMY_STATE_ENTRY(DAMAGED, Damaged),
		ENEMY_STATE_ENTRY(DESPAWNED, Despawned),
	};
	return stateTable;
}

Enemy::~Enemy()
{
	ColliderManager::GetInstance()->RemoveCollider(collider_.get());
}

void Enemy::Init()
{
	RegisterFromDefaultTable(this);

	// 敵タイプをランダムに決定（現在は4種類）
	enemyType_ = Random::GetInstance()->Int(0, 3);

	// 3Dオブジェクト生成
	obj_ = std::make_unique<Object3d>();
	obj_->Initialize();

	using namespace EnemyAttack;

	// タイプに応じたモデルと攻撃パターンの設定
	if (enemyType_ == 0)
	{
		// 通常ショット
		obj_->SetModel("crystal1.obj");
		SetAttackStrategy(std::make_unique<NormalAttackStrategy>());
	}
	else if (enemyType_ == 1)
	{
		// 垂直2点
		obj_->SetModel("crystal2.obj");
		SetAttackStrategy(std::make_unique<VerticalSplitAttackStrategy>());
	}
	else if (enemyType_ == 2)
	{
		// 水平4点
		obj_->SetModel("crystal4.obj");
		SetAttackStrategy(std::make_unique<HorizontalSplitAttackStrategy>());
	}
	else if (enemyType_ == 3)
	{
		// 3角形
		obj_->SetModel("crystal.obj");
		SetAttackStrategy(std::make_unique<TriangleAttackStrategy>());
	}
	obj_->SetColor({ 1, 1, 1, 1 });
	
	// トランスフォーム初期化
	worldTransform_.Initialize();
	worldTransform_.SetScale(defaultScale_);
	worldTransform_.Update();
	worldTransform_.SetUseQuaternion(true);

	// コライダーの生成
	collider_ = std::make_unique<EnemyCollider>(
		static_cast<uint32_t>(ColliderTypeID::ENEMY),
		GetWorldPosition(),
		defaultScale_.x,
		this
	);
	ColliderManager::GetInstance()->AddCollider(collider_.get());

	// 攻撃開始までのタイマーをランダムに設定
	std::mt19937 gen(rd());
	std::uniform_real_distribution<float> dist(0.2f, 1.0f);
	bulletTimer_ = dist(gen);


	// 出現演出へ遷移
	ChangeState(EnemyState::ENTERING);
}

void Enemy::Update()
{
	deltaTime_ = Timer::GetInstance()->GetDeltaTime();

	// 寿命管理（出現後の生存時間）
	lifeTime_ -= deltaTime_;
	if(lifeTime_ > 0)
	{
		// カメラ移動に伴う相対位置更新
		worldTransform_.SetTranslation(worldTransform_.GetTranslation() - camera_->GetDeltaTranslate());
		
		// ステート更新
		UpdateState(deltaTime_);

		// 行列更新と回転処理
		UpdateTransform();
		Rotate();
		
		// コライダー位置同期
		collider_->Update(GetWorldPosition());
	}
	else
	{
		// 寿命尽きれば死亡
		isDead_ = true;
	}
}

void Enemy::UpdateTransform()
{
	worldTransform_.Update();
}

void Enemy::Draw()
{
	obj_->Draw(worldTransform_);
}

void Enemy::Pop()
{
	worldTransform_.SetScale(ZeroScale);
	worldTransform_.Update();
	emitter.transform.translate = GetWorldPosition();
	emitter.transform.translate.y += 0.1f;
	emitter.transform.scale = { 0.5f,0.4f,0.5f };
	emitter.count = 1;
	ParticleManager::GetInstance()->SetEmitter(2, emitter);
	ParticleManager::GetInstance()->TriggerEmit(2, true);
}

void Enemy::OnCollision()
{
	// HP減少
	--hitPoint_;

	if (hitPoint_ > 0) // まだ生きている場合
	{
		GameAudio::GetInstance()->Play("damageE", false, SoundCategory::SE);
		ChangeState(EnemyState::DAMAGED);
	}
	else // HPが0以下になった場合
	{
		GameAudio::GetInstance()->Play("gekiha", false, SoundCategory::SE);
		ChangeState(EnemyState::DESPAWNED);
	}
}

void Enemy::IsShot()
{
	// 攻撃クールタイムのリセット
	bulletTimer_ = kBulletCoolTime_;
	if (!isInGame_) return;

	// 攻撃音再生
	GameAudio::GetInstance()->Play("attack", false, SoundCategory::SE);

	// 設定された攻撃パターンを実行
	if (attackStrategy_)
	{
		attackStrategy_->Attack(GetWorldPosition(), targetPos_, bulletManager_);
	}
}

void Enemy::Rotate()
{
	Vector3 playerPos = targetPos_;
	Vector3 enemyPos = worldTransform_.GetTranslation();

	Vector3 toPlayer = playerPos - enemyPos;
	if (Length(toPlayer) < 1e-6f) return;

	toPlayer = Normalize(toPlayer);

	// --- Yaw（左右） ---
	// プレイヤー方向へのY軸回転
	float yaw = std::atan2(toPlayer.x, toPlayer.z);

	// --- Pitch（上下） ---
	// プレイヤー方向へのX軸回転
	float lenXZ = std::sqrt(toPlayer.x * toPlayer.x + toPlayer.z * toPlayer.z);
	float pitch = std::atan2(-toPlayer.y, lenXZ);

	// --- Roll（必要なら） ---
	float roll = 0.0f;   // ベースは0

	// --- Euler → Quaternion（LH / Row-major / DirectX用） ---
	// 攻撃時の追加回転オフセット等を加味してクォータニオン合成
	Quaternion qYaw = MakeRotateAxisAngleQuaternion({ 0,1,0 }, yaw + shotYaw_);
	Quaternion qPitch = MakeRotateAxisAngleQuaternion({ 1,0,0 }, pitch + shotPitch_);
	Quaternion qRoll = MakeRotateAxisAngleQuaternion({ 0,0,1 }, roll + roll_ + shotRoll_);

	Quaternion q = Multiply(qYaw, Multiply(qPitch, qRoll));

	// 回転適用
	worldTransform_.SetRotateQuaternion(Normalize(q));
}

void Enemy::InitDespawned()
{
	collider_->SetTypeID(static_cast<uint32_t>(ColliderTypeID::NONE));

	// 死亡通知
	if (listener_ && isInGame_)
	{
		listener_->OnEnemyDied(this);
	}
	CameraShake::ShakeParams params;
	params.duration = 0.1f;
	params.amplitude = 0.1f;
	params.frequency = 20.0f;
	camera_->StartShake(params);

	// 爆発エフェクト
	IParticleRenderer::Emitter e;
	e.transform.translate = GetWorldPosition();
	e.count = 20;
	e.frequency = 5.0f;
	e.transform.scale = { 0.3f, 0.3f, 0.3f };
	ParticleManager::GetInstance()->SetEmitter(4, e);
	ParticleManager::GetInstance()->TriggerEmit(4, true);

	// リング
	IParticleRenderer::Emitter eR;
	eR.transform.translate = GetWorldPosition();
	eR.count = 1;
	eR.frequency = 5.0f;
	eR.transform.scale = { 0.5f, 0.5f, 0.5f };
	ParticleManager::GetInstance()->SetEmitter(1, eR);
	ParticleManager::GetInstance()->TriggerEmit(1, true);

	// 破片
	IParticleRenderer::Emitter eD;
	eD.velocity = { 0.0f, 2.0f, 0.0f };
	eD.transform.translate = GetWorldPosition();
	eD.count = 30;
	eD.frequency = 5.0f;
	eD.transform.scale = { 0.1f, 0.1f, 0.1f };
	eD.randomVel = true;
	ParticleManager::GetInstance()->SetEmitter(5, eD);
	ParticleManager::GetInstance()->TriggerEmit(5, true);
}
