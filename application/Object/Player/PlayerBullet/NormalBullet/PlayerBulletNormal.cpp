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
	worldTransform_.TransferMatrix();

	collider_ = std::make_unique<PBulletCollider>(
		static_cast<uint32_t>(ColliderTypeID::PLAYER),
		GetWorldPosition(),
		scale_,
		this
	);
	ColliderManager::GetInstance()->AddCollider(collider_.get());
	ChangeState(NormalBulletState::LINER);

	defaultSpeed_ = 12.0f;
}

void PlayerBulletNormal::Update()
{
	UpdateState(deltaTime_);
	collider_->Update(GetWorldPosition());
}

void PlayerBulletNormal::Draw()
{
	obj_->Draw(worldTransform_);
}

