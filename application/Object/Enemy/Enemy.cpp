#include "Enemy.h"
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
	obj_->SetColor({ 1.0f,0.0f,0.0f,1.0f });
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
	if (popTimer_ > 0)
	{
		popTimer_ -= deltaTime_;
		if (popTimer_ < 0)
		{
			popTimer_ = 0;
		}
		float t = popTimer_ / kPopTime_;
		t = 1.0f - t;
		worldTransform_.scale_ = Lerp(ZeroScale, defaultScale_, easeOutBounce(t));
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

float easeOutBounce(float x) {
	const float n1 = 7.5625f;
	const float d1 = 2.75f;

	if (x < 1.0f / d1) {
		return n1 * x * x;
	}
	else if (x < 2.0f / d1) {
		x -= 1.5f / d1;
		return n1 * x * x + 0.75f;
	}
	else if (x < 2.5f / d1) {
		x -= 2.25f / d1;
		return n1 * x * x + 0.9375f;
	}
	else {
		x -= 2.625f / d1;
		return n1 * x * x + 0.984375f;
	}
}
