#include "Player.h"
#include "ColliderManager.h"
#include "Input.h"
#include "Timer.h"
#include "Quaternion.h"

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
	obj_->SetIsLighting(false);
	obj_->SetColor({ 0.2f, 0.2f, 1.0f, 1.0f });
	worldTransform_.Initialize();
	worldTransform_.scale_ = { scale_, scale_, scale_ };
	worldTransform_.useQuaternion_ = true; // プレイヤーではQuaternion使うようにする
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

	obj_->SetColor({ 0.2f, 0.2f, 1.0f, 1.0f });

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
	inputDir_ = {};

	if (input_->PushKey(DIK_W)) inputDir_.y += 1.0f;
	if (input_->PushKey(DIK_S)) inputDir_.y -= 1.0f;
	if (input_->PushKey(DIK_A)) inputDir_.x -= 1.0f;
	if (input_->PushKey(DIK_D)) inputDir_.x += 1.0f;

	if (Length(inputDir_) != 0) 
	{
		inputDir_ = Normalize(inputDir_);
		screenOffset_ += inputDir_ * speed_ * deltaTime_;
	}
	ClampOffset();

	Vector3 worldPos = ConvertScreenOffsetToWorld(screenOffset_);
	worldTransform_.translation_ = worldPos;

	StartBarrelRoll();
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
	// カメラの正面方向（ワールド空間）
	Vector3 camForward = Normalize(camera_->GetForward());
	Vector3 worldUp = { 0.0f, 1.0f, 0.0f };

	// オイラー角（Yaw → Pitch）
	yaw = std::atan2(camForward.x, camForward.z);
	float lenXZ = std::sqrt(camForward.x * camForward.x + camForward.z * camForward.z);
	pitch = std::atan2(-camForward.y, lenXZ);

	// 回転クォータニオン（Yaw → Pitch）
	Quaternion qYaw = MakeRotateAxisAngleQuaternion({ 0, 1, 0 }, yaw);
	Quaternion qPitch = MakeRotateAxisAngleQuaternion({ 1, 0, 0 }, pitch);

	// ロール回転（カメラforward軸に沿って回す）
	Quaternion qRoll = MakeRotateAxisAngleQuaternion(camForward, roll + camera_->GetRotate().z);

	// 最終合成：Roll * Yaw * Pitch
	Quaternion qFinal = Multiply(Multiply(qRoll, qYaw), qPitch);

	worldTransform_.rotationQ_ = qFinal;
	worldTransform_.TransferMatrix();
}

void Player::DebugGUI()
{
#ifdef _DEBUG
	ImGui::Begin("Player");
	ImGui::DragFloat2("ScreenOffset", &screenOffset_.x);

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
#ifdef _DEBUG
	reticleObj_ = std::make_unique<Object3d>();
	reticleObj_->Initialize();
	reticleObj_->SetModel("cube.obj");
	reticleObj_->SetIsLighting(false);
	reticleObj_->SetColor({ 0,1,0,1 });
	reticleWT_.Initialize();
	reticleWT_.scale_ = { scale_, scale_, scale_ };
	reticleWT_.TransferMatrix();
#endif // _DEBUG
}

void Player::TestReticleUpdate()
{
#ifdef _DEBUG
	reticleWT_.rotation_ = worldTransform_.rotation_;
	reticleWT_.translation_ = reticle_->GetTarget();
	reticleWT_.TransferMatrix();
#endif // _DEBUG
}

void Player::TestReticleDraw()
{
#ifdef _DEBUG
	reticleObj_->Draw(reticleWT_);
#endif // _DEBUG
}
