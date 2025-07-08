#include "Player.h"
#include "ColliderManager.h"
#include "Input.h"

const std::vector<StateMachine<Player, PlayerState>::StateFunctionSet>& Player::GetStateTable()
{
	static const std::vector<StateFunctionSet> stateTable = {
	{ PlayerState::IDLE,		 &Player::InitIdle,		   &Player::UpdateIdle,		   &Player::ExitIdle },
	{ PlayerState::ROOT,		 &Player::InitRoot,		   &Player::UpdateRoot,		   &Player::ExitRoot },
	{ PlayerState::BOOST,		 &Player::InitBoost,	   &Player::UpdateBoost,	   &Player::ExitBoost },
	{ PlayerState::BARREL_ROLL,	 &Player::InitBarrelRoll,  &Player::UpdateBarrelRoll,  &Player::ExitBarrelRoll },
	{ PlayerState::TAKE_DAMAGE,	 &Player::InitTakeDamage,  &Player::UpdateTakeDamage,  &Player::ExitTakeDamage },
	{ PlayerState::DEAD,		 &Player::InitDead,		   &Player::UpdateDead,		   &Player::ExitDead },
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
	
}

void Player::Update()
{
	UpdateState(1.0f / 60.0f);
	RotationOffset();
	worldTransform_.TransferMatrix();
	collider_->Update(GetWorldPosition());

#ifdef _DEBUG
	ImGui::Begin("Player");
	Vector3 pos = GetWorldPosition();
	ImGui::DragFloat3("pos", &pos.x);
	ImGui::End();
#endif // _DEBUG

}

void Player::Draw()
{
	obj_->Draw(worldTransform_);
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
	Vector3 camForward = Normalize(camera_->GetForward());

	// Z軸向き（前方）からY軸回転（Yaw）を計算（XZ平面で）
	float yaw = std::atan2(camForward.x, camForward.z);

	// 上下回転（Pitch）も反映したいならY除いたベクトル長からPitchを算出
	float lenXZ = std::sqrt(camForward.x * camForward.x + camForward.z * camForward.z);
	float pitch = std::atan2(-camForward.y, lenXZ);

	// Roll（横傾き）は普通は 0 でOK（必要な場合だけ）
	float roll = 0.0f;

	// プレイヤーに回転を適用（Y軸回転のみ、またはX,Y）
	worldTransform_.rotation_ = { pitch, yaw, roll }; // ← 自由に調整可能
	worldTransform_.TransferMatrix();

}
