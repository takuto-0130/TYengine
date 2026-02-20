#include "PlayerBulletNormal.h"
#include "../../../ColliderTypeID/ColliderTypeID.h"
#include "Timer.h"
#include "ColliderManager.h"

using namespace TYEngine::Utility;
using namespace TYEngine::Graphics;

#define NORMAL_BULLET_ENTRY(stateEnum, funcName) \
    STATE_ENTRY_FOR(PlayerBulletNormal, stateEnum, funcName)

const std::vector<PlayerBulletNormal::StateFunctionSet>& PlayerBulletNormal::GetStateTable()
{
	using enum NormalBulletState;
	static const std::vector<StateFunctionSet> stateTable = {
		NORMAL_BULLET_ENTRY(LINEAR, Linear),
		NORMAL_BULLET_ENTRY(AFTER_COLLISION, AfterCollision),
	};
	return stateTable;
}

PlayerBulletNormal::PlayerBulletNormal()
{
	stateMachine_.RegisterFromDefaultTable(this);
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
	
	// カメラ移動分の補正（必要に応じて）
	worldTransform_.SetTranslation(worldTransform_.GetTranslation() - camera_->GetDeltaTranslate());
	
	// ステート更新（移動処理などはここで行われる）
	stateMachine_.UpdateState(deltaTime_);
	
	// コライダー同期
	collider_->Update(GetWorldPosition());
}

void PlayerBulletNormal::Draw()
{
	obj_->Draw(worldTransform_);
}

//////////////////// Linear ////////////////////
void PlayerBulletNormal::InitLinear()
{}

void PlayerBulletNormal::UpdateLinear()
{
	// 寿命を超えたら死亡フラグを true にし、削除対象とする
	if (stateMachine_.GetStateElapsedTime() > lifeTime_)
	{
		isDead_ = true;
	}

	// 移動処理
	Move();
	// 進行方向に合わせて回転
	RotationDirection();

	worldTransform_.Update();
}

void PlayerBulletNormal::ExitLinear()
{}

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

//////////////////// AfterCollision ////////////////////
void PlayerBulletNormal::InitAfterCollision()
{
	// 着弾後は即座に消滅（エフェクトなどはOnCollisionで生成済み）
	isDead_ = true;
}

void PlayerBulletNormal::UpdateAfterCollision()
{}

void PlayerBulletNormal::ExitAfterCollision()
{}

