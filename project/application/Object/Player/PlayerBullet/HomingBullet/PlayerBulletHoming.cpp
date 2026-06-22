#include "PlayerBulletHoming.h"
#include "../../../ColliderTypeID/ColliderTypeID.h"
#include "Timer.h"
#include "ColliderManager.h"
#include "../../../Enemy/EnemyManager/EnemyManager.h"

using namespace TYEngine::Utility;
using namespace TYEngine::Graphics;

// マクロ・テーブル削除

PlayerBulletHoming::PlayerBulletHoming()
{
	stateMachine_.RegisterState<HomingBulletStateShot>(HomingBulletState::SHOT, "Shot");
	stateMachine_.RegisterState<HomingBulletStateAfterCollision>(HomingBulletState::AFTER_COLLISION, "AfterCollision");
}

PlayerBulletHoming::~PlayerBulletHoming()
{
	ColliderManager::GetInstance()->RemoveCollider(collider_.get());
}

void PlayerBulletHoming::Init()
{
	// 3Dモデル生成
	obj_ = std::make_unique<Object3d>();
	obj_->Initialize();
	obj_->SetModel("unitSphere.obj");
	obj_->SetColor(jm_->Get<Vector4>("bullets.BulletHoming.Color"));

	colliderScale_ = jm_->Get<float>("bullets.BulletHoming.colliderScale");
	scale_ = jm_->Get<float>("bullets.BulletHoming.scale");
	defaultSpeed_ = jm_->Get<float>("bullets.BulletHoming.defaultSpeed");
	lifeTime_ = jm_->Get<float>("bullets.BulletHoming.lifeTime");

	minHoming_ = jm_->Get<float>("bullets.BulletHoming.minHoming");
	maxHoming_ = jm_->Get<float>("bullets.BulletHoming.maxHoming");
	homingTime_ = jm_->Get<float>("bullets.BulletHoming.HomingTime");

	// トランスフォーム初期化
	worldTransform_.Initialize();
	worldTransform_.SetScale(scale_ * jm_->Get<Vector3>("bullets.BulletHoming.scale3"));
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
	stateMachine_.ChangeState(HomingBulletState::SHOT);
}

void PlayerBulletHoming::Update()
{
	deltaTime_ = Timer::GetInstance()->GetDeltaTime();

	// カメラ移動分の補正（必要に応じて）
	worldTransform_.SetTranslation(worldTransform_.GetTranslation() - camera_->GetDeltaTranslate());

	// ステート更新（移動処理などはここで行われる）
	stateMachine_.UpdateState(*this, deltaTime_);

	// コライダー同期
	collider_->Update(GetWorldPosition());
}

void PlayerBulletHoming::Draw()
{
	obj_->Draw(worldTransform_);
}


/////////////////////////////////////////////////////////
// --- 状態クラスのメソッド実装 ---
void HomingBulletStateShot::Init(PlayerBulletHoming&) {}
void HomingBulletStateShot::Update(PlayerBulletHoming& owner, float)
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
void HomingBulletStateShot::Exit(PlayerBulletHoming&) {}

void PlayerBulletHoming::Move()
{
	if(enemyMgr_)
	{
		if(target_)
		{
			Enemy* enemyTarget = static_cast<Enemy*>(target_);
			if (enemyMgr_->IsValidEnemy(enemyTarget))
			{
				if(enemyMgr_->IsActive(enemyTarget))
				{
					Vector3 idealDir = Normalize(target_->GetWorldPosition() - GetWorldPosition());

					// 時間経過でホーミング係数を強くする
					// GetStateElapsedTime() は発射からの経過時間
					float timeRatio = stateMachine_.GetStateElapsedTime() / homingTime_; // homingTime_ を経過すると最大ホーミング力になる
					timeRatio = std::clamp(timeRatio, 0.0f, homingTime_);

					// 初期の弱いホーミング力から、最大ホーミング力へ
					float currentHomingT = std::lerp(minHoming_, maxHoming_, timeRatio);

					// 現在の進行方向 direction_ を idealDir に向かって球面線形補間する
					direction_ = Normalize(Slerp(direction_, idealDir, currentHomingT));
				}
			}
			else
			{
				target_ = nullptr;
			}
		}
	}
	// 速度ベクトルの計算
	velocity_ = direction_ * defaultSpeed_ * deltaTime_;

	// 座標更新
	worldTransform_.SetTranslation(worldTransform_.GetTranslation() + velocity_);

}

void PlayerBulletHoming::RotationDirection()
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


/////////////////////////////////////////////////////////////////
void HomingBulletStateAfterCollision::Init(PlayerBulletHoming& owner)
{
	// 着弾後は即座に消滅（エフェクトなどはOnCollisionで生成済み）
	owner.isDead_ = true;
}
void HomingBulletStateAfterCollision::Update(PlayerBulletHoming&, float) {}
void HomingBulletStateAfterCollision::Exit(PlayerBulletHoming&) {}



