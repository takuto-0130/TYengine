#include "PlayerBulletNormal.h"
#include "../../../ColliderTypeID/ColliderTypeID.h"
#include "Timer.h"
#include "ColliderManager.h"


#define NORMAL_BULLET_ENTRY(stateEnum, funcName) \
    STATE_ENTRY_FOR(PlayerBulletNormal, stateEnum, funcName)

const std::vector<StateMachine<PlayerBulletNormal, NormalBulletState>::StateFunctionSet>& PlayerBulletNormal::GetStateTable()
{
	using enum NormalBulletState;
	static const std::vector<StateFunctionSet> stateTable = {
		NORMAL_BULLET_ENTRY(LINER, Liner),
		NORMAL_BULLET_ENTRY(AFTER_COLLISION, AfterCollision),
	};
	return stateTable;
}

PlayerBulletNormal::PlayerBulletNormal()
{
	RegisterFromDefaultTable(this);
}

PlayerBulletNormal::~PlayerBulletNormal()
{
	ColliderManager::GetInstance()->RemoveCollider(collider_.get());
}

void PlayerBulletNormal::Init()
{
	obj_ = std::make_unique<Object3d>();
	obj_->Initialize();
	obj_->SetModel("unitSphere.obj");
	obj_->SetColor({ 0.2f, 0.2f, 1.0f, 1.0f });
	worldTransform_.Initialize();
	worldTransform_.colliderScale_ = { scale_, scale_, scale_ };
	worldTransform_.TransferMatrix();

	collider_ = std::make_unique<PBulletCollider>(
		static_cast<uint32_t>(ColliderTypeID::P_BULLET),
		GetWorldPosition(),
		colliderScale_,
		this
	);
	ColliderManager::GetInstance()->AddCollider(collider_.get());
	ChangeState(NormalBulletState::LINER);

	defaultSpeed_ = 25.0f;
}

void PlayerBulletNormal::Update()
{
	deltaTime_ = Timer::GetInstance()->GetDeltaTime();
	worldTransform_.translation_ -= camera_->GetDeltaTranslate();
	UpdateState(deltaTime_);
	collider_->Update(GetWorldPosition());
}

void PlayerBulletNormal::Draw()
{
	obj_->Draw(worldTransform_);
}

