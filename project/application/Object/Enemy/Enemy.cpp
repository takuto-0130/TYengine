#include "Enemy.h"
#include "./EnemyBullet/EnemyBulletManager.h"
#include "Timer.h"
#include "Ease.h"
#include "ColliderManager.h"
#include "ParticleManager.h"
#include "EnemyBullet/Liner/Liner.h"

Enemy::~Enemy()
{
	ColliderManager::GetInstance()->RemoveCollider(collider_.get());
}

void Enemy::Init()
{
	obj_ = std::make_unique<Object3d>();
	obj_->Initialize();
	obj_->SetModel("crystal.obj");
	obj_->SetColor({ 1, 1, 1, 1 });
	worldTransform_.Initialize();
	worldTransform_.colliderScale_ = defaultScale_;
	worldTransform_.TransferMatrix();

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
}

void Enemy::Update()
{
	deltaTime_ = Timer::GetInstance()->GetDeltaTime();

	if (popTimer_ > 0)
	{
		popTimer_ -= deltaTime_;
		if (popTimer_ < 0)
		{
			popTimer_ = 0;
		}
		float t = popTimer_ / kPopTime_;
		t = 1.0f - t;
		worldTransform_.colliderScale_ = Lerp(ZeroScale, defaultScale_, EaseFixed::InOutBounce(t));
	}
	else if (bulletTimer_ > 0.0f)
	{
		bulletTimer_ -= deltaTime_;
		if (bulletTimer_ >= 1.5f)
		{
			float t = bulletTimer_;
			if (t < 1.5f)
			{
				t = 1.5f;
			}
			worldTransform_.colliderScale_ = Lerp(defaultScale_, upScale_, EaseFixed::InOutBounce(t - 1.5f));
		}
	}
	else if (bulletTimer_ <= 0.0f)
	{
		IsShot();
	}


	UpdateTransform();
	Rotate();

	collider_->Update(GetWorldPosition());
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
	popTimer_ = kPopTime_;
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
	isDead_ = true;

	if (listener_) {
		listener_->OnEnemyDied(this);
	}
}

void Enemy::IsShot()
{
	bulletTimer_ = kBulletCoolTime_;
	std::unique_ptr<EnemyBullet::Liner> bullet = std::make_unique<EnemyBullet::Liner>();
	bullet->Init();
	bullet->SetTranslation(GetWorldPosition());
	bullet->SetShotDirection(Normalize(targetPos_ - GetWorldPosition()));
	bulletManager_->AddBullet(std::move(bullet));
}

void Enemy::Rotate()
{
	Vector3 playerPos = targetPos_;
	Vector3 enemyPos = worldTransform_.translation_;

	// プレイヤー方向へのベクトル
	Vector3 toPlayer = playerPos - enemyPos;

	// ゼロベクトル付近なら何もしない
	const float eps = 1e-6f;
	if (Length(toPlayer) < eps) return;

	toPlayer = Normalize(playerPos - enemyPos);

	// --- Euler角の計算 ---
	// Yaw (水平回転) : XZ平面上の向き
	float yaw = std::atan2(toPlayer.x, toPlayer.z);

	// Pitch (上下回転) : 前方向とY軸成分から算出
	float lenXZ = std::sqrt(toPlayer.x * toPlayer.x + toPlayer.z * toPlayer.z);
	float pitch = std::atan2(-toPlayer.y, lenXZ);

	// Rollは不要（0でOK）
	float roll = 0.0f;

	// 回転に格納
	worldTransform_.rotation_ = { pitch, yaw, roll };
}
