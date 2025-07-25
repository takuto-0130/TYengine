#include "Enemy.h"
#include "Timer.h"
#include "Ease.h"
#include "ColliderManager.h"
#include "ParticleManager.h"

Enemy::~Enemy()
{
	ColliderManager::GetInstance()->RemoveCollider(collider_.get());
}

void Enemy::Init()
{
	obj_ = std::make_unique<Object3d>();
	obj_->Initialize();
	obj_->SetModel("unitSphere.obj");
	obj_->SetEnvironmentCoefficient(1.0f);
	worldTransform_.Initialize();
	worldTransform_.scale_ = defaultScale_;
	worldTransform_.TransferMatrix();

	collider_ = std::make_unique<EnemyCollider>(
		static_cast<uint32_t>(ColliderTypeID::ENEMY),
		GetWorldPosition(),
		defaultScale_.x,
		this
	);
	ColliderManager::GetInstance()->AddCollider(collider_.get());
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
		worldTransform_.scale_ = Lerp(ZeroScale, defaultScale_, EaseFixed::InOutBounce(t));
	}


	UpdateTransform();

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
	worldTransform_.scale_ = ZeroScale;
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
