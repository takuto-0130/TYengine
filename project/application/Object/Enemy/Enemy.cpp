#include "Enemy.h"
#include "./EnemyBullet/EnemyBulletManager.h"
#include "AttackStrategy/AttackStrategies.h"
#include "Timer.h"
#include "Ease.h"
#include "ColliderManager.h"
#include "ParticleManager.h"
#include "EnemyBullet/Linear/Linear.h"
#include "Random.h"
#include "../../AppSystem/Audio/GameAudio.h"

using namespace TYEngine::Utility;
using namespace TYEngine::Graphics;
using namespace TYEngine::Effect;
using namespace TYEngine::CameraSystem;

Enemy::~Enemy()
{
	ColliderManager::GetInstance()->RemoveCollider(collider_.get());
}

void Enemy::Init()
{
	stateMachine_.RegisterState<EnemyStatePreEnter>(EnemyState::PRE_ENTER, "PreEnter");
	stateMachine_.RegisterState<EnemyStateEntering>(EnemyState::ENTERING, "Entering");
	stateMachine_.RegisterState<EnemyStateActive>(EnemyState::ACTIVE, "Active");
	stateMachine_.RegisterState<EnemyStateExiting>(EnemyState::EXITING, "Exiting");
	stateMachine_.RegisterState<EnemyStateDamaged>(EnemyState::DAMAGED, "Damaged");
	stateMachine_.RegisterState<EnemyStateDespawned>(EnemyState::DESPAWNED, "Despawned");

	// 数値を適用
	popTime_ = 1.0f;
	bulletCoolTime_ = 2.0f;
	bulletTimer_ = 0.0f;
	defaultScale_ = { 0.3f, 0.3f, 0.3f };
	upScale_ = { 0.45f, 0.45f, 0.45f };
	lifeTime_ = 15.0f;



	// 敵タイプをランダムに決定（現在は4種類）
	enemyType_ = Random::GetInstance()->Int(0, EnemyType::Triangle);

	// 3Dオブジェクト生成
	obj_ = std::make_unique<Object3d>();
	obj_->Initialize();

	using namespace EnemyAttack;

	// タイプに応じたモデルと攻撃パターンの設定
	if (enemyType_ == EnemyType::Normal)
	{
		// 通常ショット
		obj_->SetModel("crystal1.obj");
		SetAttackStrategy(std::make_unique<NormalAttackStrategy>());
	}
	else if (enemyType_ == EnemyType::Vertical)
	{
		// 垂直2点
		obj_->SetModel("crystal2.obj");
		SetAttackStrategy(std::make_unique<VerticalSplitAttackStrategy>());
	}
	else if (enemyType_ == EnemyType::Horizontal)
	{
		// 水平4点
		obj_->SetModel("crystal4.obj");
		SetAttackStrategy(std::make_unique<HorizontalSplitAttackStrategy>());
	}
	else if (enemyType_ == EnemyType::Triangle)
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
	stateMachine_.ChangeState(EnemyState::ENTERING);
}

void Enemy::Update()
{
	deltaTime_ = Timer::GetInstance()->GetDeltaTime();

	// 寿命管理（出現後の生存時間）
	lifeTime_ -= deltaTime_;
	if(lifeTime_ > 0)
	{
		// カメラ移動に伴う相対位置更新
		/*worldTransform_.SetTranslation(worldTransform_.GetTranslation() - camera_->GetDeltaTranslate());*/
		
		// ステート更新
		stateMachine_.UpdateState(*this, deltaTime_);

		// 行列更新と回転処理
		UpdateTransform();
		Rotate();
		
		// コライダー位置同期
		collider_->Update(GetWorldPosition());
		Vector2 pos = {};
		if (!camera_->WorldToNDC(GetWorldPosition(), pos))
		{
			// 画面外に出たら死亡
			isDead_ = true;
		}
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
		stateMachine_.ChangeState(EnemyState::DAMAGED);
	}
	else // HPが0以下になった場合
	{
		GameAudio::GetInstance()->Play("gekiha", false, SoundCategory::SE);
		stateMachine_.ChangeState(EnemyState::DESPAWNED);
	}
}

void Enemy::IsShot()
{
	// 攻撃クールタイムのリセット
	bulletTimer_ = bulletCoolTime_;
	if (!isInGame_) return;

	// 攻撃音再生
	GameAudio::GetInstance()->Play("attack", false, SoundCategory::SE);

	// 設定された攻撃パターンを実行
	/*if (attackStrategy_)
	{
		attackStrategy_->Attack(GetWorldPosition(), targetPos_, bulletManager_);
	}*/
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

// --- 状態クラスのメソッド実装 ---
void EnemyStatePreEnter::Init(Enemy&) {}
void EnemyStatePreEnter::Update(Enemy&, float) {}
void EnemyStatePreEnter::Exit(Enemy&) {}

void EnemyStateEntering::Init(Enemy&) {}
void EnemyStateEntering::Update(Enemy& owner, float)
{
	float t = GetElapsed() / owner.popTime_;
	if (t <= 1.0f)
	{
		owner.worldTransform_.SetScale(Lerp(owner.ZeroScale, owner.defaultScale_, EaseFixed::InOutBounce(t)));
	}
	else
	{
		RequestStateChange(EnemyState::ACTIVE);
	}
}
void EnemyStateEntering::Exit(Enemy& owner)
{
	owner.worldTransform_.SetScale(owner.defaultScale_);
}

void EnemyStateActive::Init(Enemy&) {}
void EnemyStateActive::Update(Enemy& owner, float deltaTime)
{
	if (owner.bulletTimer_ > 0.0f)
	{
		owner.bulletTimer_ -= deltaTime;
		if (owner.bulletTimer_ >= 1.5f)
		{
			float t = owner.bulletTimer_;
			if (t < 1.5f)
			{
				t = 1.5f;
			}
			//owner.worldTransform_.SetScale(Lerp(owner.defaultScale_, owner.upScale_, EaseFixed::InOutBounce(t - 1.5f)));
		}

		if (owner.bulletTimer_ <= 0.5f)
		{
			float t = 1.0f - (owner.bulletTimer_ / 0.5f);
			if (owner.enemyType_ == 1)
			{
				// 垂直2点
				owner.shotPitch_ = Lerp(0.0f, 2.0f * std::numbers::pi_v<float>, EaseFixed::InBack(t));
			}
			else if (owner.enemyType_ == 2)
			{
				// 水平4点
				owner.shotYaw_ = Lerp(0.0f, 2.0f * std::numbers::pi_v<float>, EaseFixed::InBack(t));
			}
			else if (owner.enemyType_ == 3)
			{
				// 3角形
				owner.shotRoll_ = Lerp(0.0f, 2.0f * std::numbers::pi_v<float>, EaseFixed::InBack(t));
			}
		}
	}
	else if (owner.bulletTimer_ <= 0.0f)
	{
		owner.shotYaw_ = 0;
		owner.shotPitch_ = 0;
		owner.shotRoll_ = 0;
		owner.IsShot();
	}
}
void EnemyStateActive::Exit(Enemy&) {}

void EnemyStateExiting::Init(Enemy&) {}
void EnemyStateExiting::Update(Enemy&, float) {}
void EnemyStateExiting::Exit(Enemy&) {}

void EnemyStateDamaged::Init(Enemy& owner)
{
	owner.obj_->SetAddColor({ 1,1,1,1 });
	owner.roll_ = 0.1f;
}
void EnemyStateDamaged::Update(Enemy&, float)
{
	if (GetElapsed() > 0.05f)
		RequestStateChange(EnemyState::ACTIVE);
}
void EnemyStateDamaged::Exit(Enemy& owner)
{
	owner.obj_->SetAddColor({ 0,0,0,0 });
	owner.roll_ = 0.0f;
}

void EnemyStateDespawned::Init(Enemy& owner)
{
	owner.collider_->SetTypeID(static_cast<uint32_t>(ColliderTypeID::NONE));

	// 死亡通知
	if (owner.listener_ && owner.isInGame_)
	{
		owner.listener_->OnEnemyDied(&owner);
	}
	CameraShake::ShakeParams params;
	params.duration = 0.1f;
	params.amplitude = 0.1f;
	params.frequency = 20.0f;
	owner.camera_->StartShake(params);

	// 爆発エフェクト
	IParticleRenderer::Emitter e;
	e.transform.translate = owner.GetWorldPosition();
	e.count = 20;
	e.frequency = 5.0f;
	e.transform.scale = { 0.3f, 0.3f, 0.3f };
	ParticleManager::GetInstance()->SetEmitter(4, e);
	ParticleManager::GetInstance()->TriggerEmit(4, true);

	// リング
	IParticleRenderer::Emitter eR;
	eR.transform.translate = owner.GetWorldPosition();
	eR.count = 1;
	eR.frequency = 5.0f;
	eR.transform.scale = { 0.5f, 0.5f, 0.5f };
	ParticleManager::GetInstance()->SetEmitter(1, eR);
	ParticleManager::GetInstance()->TriggerEmit(1, true);

	// 破片
	IParticleRenderer::Emitter eD;
	eD.velocity = { 0.0f, 2.0f, 0.0f };
	eD.transform.translate = owner.GetWorldPosition();
	eD.count = 30;
	eD.frequency = 5.0f;
	eD.transform.scale = { 0.1f, 0.1f, 0.1f };
	eD.randomVel = true;
	ParticleManager::GetInstance()->SetEmitter(5, eD);
	ParticleManager::GetInstance()->TriggerEmit(5, true);
}
void EnemyStateDespawned::Update(Enemy& owner, float)
{
	if (GetElapsed() < 2.0f)
	{
		owner.roll_ += 0.02f;
		Vector3 pos = owner.worldTransform_.GetTranslation();
		pos.y -= 0.02f;
		owner.worldTransform_.SetTranslation(pos);
		float t = 1.0f - (GetElapsed() / 2.0f);
		owner.obj_->SetAlpha(t / 2.0f);
		owner.worldTransform_.SetScale(owner.defaultScale_ * t);
	}
	else
	{
		RequestStateChange(EnemyState::EXITING);
	}
}
void EnemyStateDespawned::Exit(Enemy& owner)
{
	owner.isDead_ = true;
}
