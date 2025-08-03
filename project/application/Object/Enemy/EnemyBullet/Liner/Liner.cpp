#include "Liner.h"
#include "../../../ColliderTypeID/ColliderTypeID.h"
#include "Timer.h"
#include "ColliderManager.h"

#ifdef _DEBUG
#include "imgui.h"
#endif // _DEBUG


using namespace EnemyBullet;


#define E_LINER_BULLET_ENTRY(stateEnum, funcName) \
    STATE_ENTRY_FOR(Liner, stateEnum, funcName)

const std::vector<StateMachine<Liner, LinerState>::StateFunctionSet>& Liner::GetStateTable()
{
	using enum LinerState;
	static const std::vector<StateFunctionSet> stateTable = {
		E_LINER_BULLET_ENTRY(SHOT, Shot),
		E_LINER_BULLET_ENTRY(AFTER_COLLISION, AfterCollision),
	};
	return stateTable;
}

Liner::Liner()
{
	RegisterFromDefaultTable(this);
}

Liner::~Liner()
{
	ColliderManager::GetInstance()->RemoveCollider(collider_.get());
}

void Liner::Init()
{

	obj_ = std::make_unique<Object3d>();
	obj_->Initialize();
	obj_->SetModel("cube.obj");
	obj_->SetColor({ 1.0f, 0.1f, 0.1f, 1.0f });
	obj_->SetIsLighting(false);
	worldTransform_.Initialize();
	worldTransform_.scale_ = { scale_, scale_, scale_ };
	worldTransform_.TransferMatrix();

	collider_ = std::make_unique<EBulletCollider>(
		static_cast<uint32_t>(ColliderTypeID::E_BULLET),
		GetWorldPosition(),
		scale_,
		this
	);
	ColliderManager::GetInstance()->AddCollider(collider_.get());
	ChangeState(LinerState::SHOT);

	defaultSpeed_ = 12.0f;
}

void Liner::Update()
{
	deltaTime_ = Timer::GetInstance()->GetDeltaTime();
	UpdateState(deltaTime_);
	collider_->Update(GetWorldPosition());
}

void Liner::Draw()
{
	obj_->Draw(worldTransform_);
}
