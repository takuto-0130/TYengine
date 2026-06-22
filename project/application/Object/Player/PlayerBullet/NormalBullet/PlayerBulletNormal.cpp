#include "PlayerBulletNormal.h"
#include "../../../ColliderTypeID/ColliderTypeID.h"
#include "Timer.h"
#include "ColliderManager.h"

using namespace TYEngine::Utility;
using namespace TYEngine::Graphics;

// マクロ・テーブル削除

PlayerBulletNormal::PlayerBulletNormal()
{
	stateMachine_.RegisterState<NormalBulletStateLinear>(NormalBulletState::LINEAR, "Linear");
	stateMachine_.RegisterState<NormalBulletStateAfterCollision>(NormalBulletState::AFTER_COLLISION, "AfterCollision");
}

PlayerBulletNormal::~PlayerBulletNormal()
{
	ColliderManager::GetInstance()->RemoveCollider(collider_.get());
}

void PlayerBulletNormal::Init()
{
	// 3Dモデル生成
	obj_ = std::make_unique<Object3d>();
	obj_->Initialize();
	obj_->SetModel("unitSphere.obj");
	obj_->SetColor(jm_->Get<Vector4>("bullets.BulletNormal.Color"));

	colliderScale_ = jm_->Get<float>("bullets.BulletNormal.colliderScale");
	scale_ = jm_->Get<float>("bullets.BulletNormal.scale");
	defaultSpeed_ = jm_->Get<float>("bullets.BulletNormal.defaultSpeed");
	lifeTime_ = jm_->Get<float>("bullets.BulletNormal.lifeTime");
	
	// トランスフォーム初期化
	worldTransform_.Initialize();
	worldTransform_.SetScale(scale_);
	worldTransform_.Update();

	// コライダー生成（Ray判定用など、適切な型を指定）
	collider_ = std::make_unique<PBulletCollider>(
		static_cast<uint32_t>(ColliderTypeID::P_BULLET),
		GetWorldPosition(),
		colliderScale_,
		this
	);
	ColliderManager::GetInstance()->AddCollider(collider_.get());
	
	// 初期状態を直線移動（LINER）に設定
	stateMachine_.ChangeState(NormalBulletState::LINEAR);

}

void PlayerBulletNormal::Update()
{
	deltaTime_ = Timer::GetInstance()->GetDeltaTime();
	
	// ステート更新（移動処理などはここで行われる）
	stateMachine_.UpdateState(*this, deltaTime_);
	
	// コライダー同期
	collider_->Update(GetWorldPosition());
}

void PlayerBulletNormal::Draw()
{
	obj_->Draw(worldTransform_);
}

// --- 状態クラスのメソッド実装 ---
void NormalBulletStateLinear::Init(PlayerBulletNormal&) {}
void NormalBulletStateLinear::Update(PlayerBulletNormal& owner, float)
{
	// 寿命を超えたら死亡フラグを true にし、削除対象とする
	if (GetElapsed() > owner.lifeTime_)
	{
		owner.isDead_ = true;
	}

	// 移動処理
	owner.Move();
	// 進行方向に合わせて回転
	owner.RotationDirection();

	owner.worldTransform_.Update();
}
void NormalBulletStateLinear::Exit(PlayerBulletNormal&) {}

void NormalBulletStateAfterCollision::Init(PlayerBulletNormal& owner)
{
	// 着弾後は即座に消滅（エフェクトなどはOnCollisionで生成済み）
	owner.isDead_ = true;
}
void NormalBulletStateAfterCollision::Update(PlayerBulletNormal&, float) {}
void NormalBulletStateAfterCollision::Exit(PlayerBulletNormal&) {}

void PlayerBulletNormal::Move()
{
	// 速度ベクトルの計算
	velocity_ = direction_ * defaultSpeed_ * deltaTime_;

	// 座標更新
	worldTransform_.SetTranslation(worldTransform_.GetTranslation() + velocity_);
}

void PlayerBulletNormal::RotationDirection()
{
	// 進行方向（Y軸回転 Yaw）を計算
	float yaw = std::atan2(direction_.x, direction_.z);

	// 進行方向（X軸回転 Pitch）を計算
	float lenXZ = std::sqrt(direction_.x * direction_.x + direction_.z * direction_.z);
	float pitch = std::atan2(-direction_.y, lenXZ);

	// Roll（横傾き）は固定
	float roll = 0.0f;

	// 回転を適用
	worldTransform_.SetRotate({ pitch, yaw, roll });
	worldTransform_.Update();
}

