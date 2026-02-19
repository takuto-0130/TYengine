#include "Linear.h"
#include "../../../ColliderTypeID/ColliderTypeID.h"
#include "Timer.h"
#include "ColliderManager.h"

#ifdef _DEBUG
#include "imgui.h"
#endif // _DEBUG

using namespace TYEngine::Utility;

#define E_LINEAR_BULLET_ENTRY(stateEnum, funcName) \
    STATE_ENTRY_FOR(Linear, stateEnum, funcName)

namespace EnemyBullet
{
	const std::vector<Linear::StateFunctionSet>& Linear::GetStateTable()
	{
		using enum LinearState;
		static const std::vector<StateFunctionSet> stateTable = {
			E_LINEAR_BULLET_ENTRY(SHOT, Shot),
			E_LINEAR_BULLET_ENTRY(AFTER_COLLISION, AfterCollision),
		};
		return stateTable;
	}

	Linear::Linear()
	{
		stateMachine_.RegisterFromDefaultTable(this);
	}

	Linear::~Linear()
	{
		ColliderManager::GetInstance()->RemoveCollider(collider_.get());
	}

	void Linear::Init()
	{
		// 3Dモデル生成
		obj_ = std::make_unique<TYEngine::Graphics::Object3d>();
		obj_->Initialize();
		obj_->SetModel("unitSphere.obj");
		obj_->SetColor({ 1.0f, 0.0f, 0.0f, 1.0f });
		obj_->SetIsLighting(true);

		// トランスフォーム初期化
		worldTransform_.Initialize();
		worldTransform_.SetScale({ colliderScale_, colliderScale_, colliderScale_ });
		worldTransform_.Update();

		// コライダー生成・登録
		collider_ = std::make_unique<EBulletCollider>(
			static_cast<uint32_t>(ColliderTypeID::E_BULLET),
			GetWorldPosition(),
			colliderScale_,
			this
		);
		ColliderManager::GetInstance()->AddCollider(collider_.get());

		// 初期ステートをSHOT（発射中）に
		stateMachine_.ChangeState(LinearState::SHOT);

		defaultSpeed_ = 20.0f;
	}

	void Linear::Update()
	{
		deltaTime_ = Timer::GetInstance()->GetDeltaTime();
		stateMachine_.UpdateState(deltaTime_);
		collider_->Update(GetWorldPosition());
	}

	void Linear::Draw()
	{
		obj_->Draw(worldTransform_);
	}
}
