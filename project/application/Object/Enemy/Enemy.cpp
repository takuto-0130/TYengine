#include "Enemy.h"
#include "./EnemyBullet/EnemyBulletManager.h"
#include "Timer.h"
#include "Ease.h"
#include "ColliderManager.h"
#include "ParticleManager.h"
#include "EnemyBullet/Liner/Liner.h"
#include "Random.h"

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

	obj_ = std::make_unique<Object3d>();
	obj_->Initialize();
	obj_->SetModel("crystal.obj");
	obj_->SetColor({ 1, 1, 1, 1 });
	worldTransform_.Initialize();
	worldTransform_.colliderScale_ = defaultScale_;
	worldTransform_.TransferMatrix();
	worldTransform_.useQuaternion_ = true;

	collider_ = std::make_unique<EnemyCollider>(
		static_cast<uint32_t>(ColliderTypeID::ENEMY),
		GetWorldPosition(),
		defaultScale_.x,
		this
	);
	ColliderManager::GetInstance()->AddCollider(collider_.get());

	std::mt19937 gen(rd());
	std::uniform_real_distribution<float> dist(0.2f, 1.0f);
	bulletTimer_ = dist(gen);


	ChangeState(EnemyState::ENTERING);
}

void Enemy::Update()
{
	deltaTime_ = Timer::GetInstance()->GetDeltaTime();

	lifeTime_ -= deltaTime_;
	if(lifeTime_ > 0)
	{
		worldTransform_.translation_ -= camera_->GetDeltaTranslate();
		UpdateState(deltaTime_);

		UpdateTransform();
		Rotate();
		collider_->Update(GetWorldPosition());
	}
	else
	{
		isDead_ = true;
	}
}

void Enemy::UpdateTransform()
{
	worldTransform_.TransferMatrix();
}

void Enemy::Draw()
{
	obj_->Draw(worldTransform_);
}

void Enemy::Pop()
{
	worldTransform_.colliderScale_ = ZeroScale;
	worldTransform_.TransferMatrix();
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
	--hitpoint_;

	if (hitpoint_ > 0) // 0より大きいなら
	{
		ChangeState(EnemyState::DAMAGED);
	}
	else // 0以下の時
	{
		ChangeState(EnemyState::DESPAWNED);
	}
}

void Enemy::IsShot()
{
	bulletTimer_ = kBulletCoolTime_;

	if (isInGame_)
	{
		std::unique_ptr<EnemyBullet::Liner> bullet = std::make_unique<EnemyBullet::Liner>();
		bullet->Init();
		bullet->SetTranslation(GetWorldPosition());
		bullet->SetShotDirection(Normalize(targetPos_ - GetWorldPosition()));
		bulletManager_->AddBullet(std::move(bullet));
	}
}

void Enemy::Rotate()
{
	Vector3 playerPos = targetPos_;
	Vector3 enemyPos = worldTransform_.translation_;

	Vector3 toPlayer = playerPos - enemyPos;
	if (Length(toPlayer) < 1e-6f) return;

	toPlayer = Normalize(toPlayer);

	// --- Yaw（左右） ---
	float yaw = std::atan2(toPlayer.x, toPlayer.z);

	// --- Pitch（上下） ---
	float lenXZ = std::sqrt(toPlayer.x * toPlayer.x + toPlayer.z * toPlayer.z);
	float pitch = std::atan2(-toPlayer.y, lenXZ);

	// --- Roll（必要なら） ---
	float roll = 0.0f;   // プレイヤーと同じならこれは 0

	// --- Euler → Quaternion（LH / Row-major / DirectX用） ---
	Quaternion qYaw = MakeRotateAxisAngleQuaternion({ 0,1,0 }, yaw);
	Quaternion qPitch = MakeRotateAxisAngleQuaternion({ 1,0,0 }, pitch);
	Quaternion qRoll = MakeRotateAxisAngleQuaternion({ 0,0,1 }, roll + roll_);

	Quaternion q = Multiply(qYaw, Multiply(qPitch, qRoll));

	worldTransform_.rotationQ_ = Normalize(q);
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
