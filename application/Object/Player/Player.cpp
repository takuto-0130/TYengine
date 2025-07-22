#include "Player.h"
#include "ColliderManager.h"
#include "Input.h"
#include "Timer.h"

#define PLAYER_STATE_ENTRY(stateEnum, funcName) \
    STATE_ENTRY_FOR(Player, stateEnum, funcName)

const std::vector<StateMachine<Player, PlayerState>::StateFunctionSet>& Player::GetStateTable()
{
	using enum PlayerState;
	static const std::vector<StateFunctionSet> stateTable = {
		PLAYER_STATE_ENTRY(IDLE, Idle),
		PLAYER_STATE_ENTRY(ROOT, Root),
		PLAYER_STATE_ENTRY(BOOST, Boost),
		PLAYER_STATE_ENTRY(BARREL_ROLL, BarrelRoll),
		PLAYER_STATE_ENTRY(TAKE_DAMAGE, TakeDamage),
		PLAYER_STATE_ENTRY(DEAD, Dead),
	};
	return stateTable;
}

Player::~Player()
{
	ColliderManager::GetInstance()->RemoveCollider(collider_.get());
}

void Player::Init()
{
	RegisterFromDefaultTable(this);
	input_ = Input::GetInstance();
	obj_ = std::make_unique<Object3d>();
	obj_->Initialize();
	obj_->SetModel("cube.obj");
	worldTransform_.Initialize();
	worldTransform_.scale_ = { scale_, scale_, scale_ };
	worldTransform_.TransferMatrix();

	collider_ = std::make_unique<PlayerCollider>(
		static_cast<uint32_t>(ColliderTypeID::PLAYER), 
		GetWorldPosition(), 
		scale_, 
		this
	);
	ColliderManager::GetInstance()->AddCollider(collider_.get());
	ChangeState(PlayerState::ROOT);

	bulletManager_ = std::make_unique<PlayerBulletManager>(this);
	bulletManager_->Init();
	
	reticle_ = std::make_unique<Reticle>(camera_);
	reticle_->Init();

	// test
	TestReticleInit();
}

void Player::Update()
{
	deltaTime_ = Timer::GetInstance()->GetDeltaTime();

	UpdateState(deltaTime_);
	RotationOffset();
	worldTransform_.TransferMatrix();
	collider_->Update(GetWorldPosition());

	Attack();
	bulletManager_->Update();

	reticle_->Update();

	// test
	TestReticleUpdate();

	DebugGUI();
}

void Player::Draw()
{
	obj_->Draw(worldTransform_);
	bulletManager_->Draw();

	// test
	TestReticleDraw();
}

void Player::Attack()
{
	if (input_->TriggerKey(DIK_SPACE))
	{
		currentBulletType_ = PlayerBulletType::NORMAL;
		Vector3 direction = Normalize(reticle_->GetTarget() - GetWorldPosition());
		bulletManager_->Fire(currentBulletType_, GetWorldPosition(), direction);
	}
}

void Player::Move()
{
	Vector2 inputDir = {};
	if (input_->PushKey(DIK_W)) inputDir.y += 1.0f;
	if (input_->PushKey(DIK_S)) inputDir.y -= 1.0f;
	if (input_->PushKey(DIK_A)) inputDir.x -= 1.0f;
	if (input_->PushKey(DIK_D)) inputDir.x += 1.0f;
	if (Length(inputDir) != 0) inputDir = Normalize(inputDir);

	screenOffset_.x += inputDir.x * speed_.x * deltaTime_;
	screenOffset_.y += inputDir.y * speed_.y * deltaTime_;
	ClampOffset();

	Vector3 worldPos = ConvertScreenOffsetToWorld(screenOffset_);
	worldTransform_.translation_ = worldPos;
}

void Player::ClampOffset()
{
	screenOffset_.x = std::clamp(screenOffset_.x, -1.0f, 1.0f);
	screenOffset_.y = std::clamp(screenOffset_.y, -1.0f, 1.0f);
}

Vector3 Player::ConvertScreenOffsetToWorld(const Vector2& offset)
{
	Vector3 camPos = camera_->GetPosition();
	Vector3 camForward = camera_->GetForward();
	Vector3 camRight = camera_->GetRight();
	Vector3 camUp = camera_->GetUp();

	

	return camPos
		+ camForward * playerDepthFromCamera_
		+ camRight * (offset.x * xRange)
		+ camUp * (offset.y * yRange);
}

void Player::RotationOffset()
{
	// カメラ前方
	Vector3 camForward = Normalize(camera_->GetForward());

	// Yaw（Y軸回転）
	float yaw = std::atan2(camForward.x, camForward.z);

	// Pitch（上下回転）
	float lenXZ = std::sqrt(camForward.x * camForward.x + camForward.z * camForward.z);
	float pitch = std::atan2(-camForward.y, lenXZ);

	// Roll（横傾き）
	float roll = 0.0f;

	// プレイヤーに回転を適用
	worldTransform_.rotation_ = { pitch, yaw, roll };
	worldTransform_.TransferMatrix();

}

void Player::DebugGUI()
{
#ifdef _DEBUG
	ImGui::Begin("Player");
	Vector3 pos = GetWorldPosition();
	ImGui::DragFloat3("pos", &pos.x);


	Vector3 dir = reticle_->GetRay().diff;
	ImGui::DragFloat3("diff", &dir.x);

	Vector3 ori = reticle_->GetRay().origin;
	ImGui::DragFloat3("origin", &ori.x);

	float dis = reticle_->GetTargetDistance();
	ImGui::DragFloat("dis", &dis);

	Vector3 target = reticle_->GetTarget();
	ImGui::DragFloat3("target", &target.x);
	ImGui::End();
#endif // _DEBUG
}

void Player::TestReticleInit()
{
	reticleObj_ = std::make_unique<Object3d>();
	reticleObj_->Initialize();
	reticleObj_->SetModel("cube.obj");
	reticleWT_.Initialize();
	reticleWT_.scale_ = { scale_, scale_, scale_ };
	reticleWT_.TransferMatrix();
}

void Player::TestReticleUpdate()
{
	reticleWT_.translation_ = reticle_->GetTarget();
	reticleWT_.TransferMatrix();
}

void Player::TestReticleDraw()
{
	reticleObj_->Draw(reticleWT_);
}
