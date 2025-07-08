#include "PlayerBulletNormal.h"
#include "../../../ColliderTypeID/ColliderTypeID.h"
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
	obj_->SetModel("cube.obj");
	worldTransform_.Initialize();
	worldTransform_.scale_ = { scale_, scale_, scale_ };

	collider_ = std::make_unique<PBulletCollider<PlayerBulletNormal>>(
		static_cast<uint32_t>(ColliderTypeID::PLAYER),
		GetWorldPosition(),
		scale_,
		this
	);
	ColliderManager::GetInstance()->AddCollider(collider_.get());
	ChangeState(NormalBulletState::LINER);
}

void PlayerBulletNormal::Update()
{
	UpdateState(deltaTime_);
}

void PlayerBulletNormal::Draw()
{
	obj_->Draw(worldTransform_);
}

