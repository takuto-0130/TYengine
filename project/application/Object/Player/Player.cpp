#include "Player.h"
#include "ColliderManager.h"
#include "Input.h"
#include "Timer.h"
#include "Quaternion.h"
#include "Effect/PlaneParticle.h"
#include "Effect/ContrailBehaviour.h"
#include "Effect/ParticleManager.h"
#include "../../AppSystem/Audio/GameAudio.h"

#define PLAYER_STATE_ENTRY(stateEnum, funcName) \
    STATE_ENTRY_FOR(Player, stateEnum, funcName)

const std::vector<StateMachine<Player, PlayerState>::StateFunctionSet>& Player::GetStateTable()
{
	using enum PlayerState;
	static const std::vector<StateFunctionSet> stateTable = 
	{
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
	ColliderManager::GetInstance()->RemoveCollider(justCollider_.get());
}

void Player::Init()
{
	RegisterFromDefaultTable(this);
	input_ = Input::GetInstance();
	obj_ = std::make_unique<Object3d>();
	obj_->Initialize();
	obj_->SetModel("jett.obj");
	obj_->SetIsLighting(false);
	obj_->SetColor({ 1.0f, 1.0f, 1.0f, 1.0f });
	worldTransform_.Initialize();
	worldTransform_.colliderScale_ = { colliderScale_, colliderScale_, colliderScale_ };
	worldTransform_.useQuaternion_ = true; // プレイヤーではQuaternion使うようにする
	worldTransform_.TransferMatrix();
	worldTransform_.parentMatrix_ = &camera_->GetWorldMatrix();
	worldTransform_.translation_.z = 4.0f;

	collider_ = std::make_unique<PlayerCollider>(
		static_cast<uint32_t>(ColliderTypeID::PLAYER), 
		GetWorldPosition(), 
		colliderScale_, 
		this
	);
	ColliderManager::GetInstance()->AddCollider(collider_.get());

	justCollider_ = std::make_unique<JustCollider>(
		static_cast<uint32_t>(ColliderTypeID::JUST_AREA),
		GetWorldPosition(),
		justScale_,
		this
	);
	ColliderManager::GetInstance()->AddCollider(justCollider_.get());


	ChangeState(PlayerState::ROOT);

	bulletManager_ = std::make_unique<PlayerBulletManager>(this);
	bulletManager_->SetCamera(camera_);
	bulletManager_->Init();
	
	reticle_ = std::make_unique<Reticle>(camera_);
	reticle_->Init();

	hitpoint_ = 9;

	// test
	TestReticleInit();
}

void Player::Update()
{
	deltaTime_ = Timer::GetInstance()->GetDeltaTime();

	obj_->SetColor({ 1.0f, 1.0f, 1.0f, 1.0f });

	UpdateState(deltaTime_);

	Vector3 back = -Normalize(Vector3(worldTransform_.matWorld_.m[2][0], worldTransform_.matWorld_.m[2][1], worldTransform_.matWorld_.m[2][2]));
	IParticleRenderer::Emitter e;
	e.transform.translate = GetWorldPosition() + back * 0.3f;
	e.transform.scale = { 0.1f ,0.1f ,0.1f };
	e.velocity = back * 3.0f;
	e.count = 1;
	e.frequency = 0.01f; // 毎フレーム発生

	ParticleManager::GetInstance()->SetEmitter(3, e);
	ParticleManager::GetInstance()->TriggerEmit(3, true);

	PostStateUpdate();
}

void Player::Draw()
{
	if(!isDead_)
	{
		obj_->Draw(worldTransform_);
		bulletManager_->Draw();

		// test
		TestReticleDraw();
	}
}


////////////////////// ちょっとおかしいので後に修正 //////////////////////
void Player::TakeDamage()
{
#ifdef _DEBUG
	// デバッグ時ダメージ処理

#else
	// 通常ダメージ処理
	--hitpoint_;
#endif // _DEBUG

	if (hitpoint_ > 0)
	{
		GameAudio::GetInstance()->Play("damageP", false, SoundCategory::SE);
		ChangeState(PlayerState::TAKE_DAMAGE);
	}
	else
	{
		OnCollision();
	}
}

void Player::OnCollision()
{
	isDead_ = true;

	GameAudio::GetInstance()->Play("gekiha", false, SoundCategory::SE);

	IParticleRenderer::Emitter e;
	e.transform.translate = GetWorldPosition();
	e.count = 20;
	e.frequency = 5.0f;
	e.transform.scale = { 0.3f, 0.3f, 0.3f };
	ParticleManager::GetInstance()->SetEmitter(4, e);

	ParticleManager::GetInstance()->TriggerEmit(4, true);
}
////////////////////// ちょっとおかしいので後に修正 //////////////////////


void Player::PostStateUpdate()
{
	//RotationOffset();
	RotationOffsetLocal();
	if(camera_->ShakeActive()) worldTransform_.translation_ -= camera_->GetShake();
	worldTransform_.TransferMatrix();
	collider_->Update(GetWorldPosition());
	justCollider_->Update(GetWorldPosition());

	Attack();
	bulletManager_->Update();

	reticle_->Update();

	// test
	TestReticleUpdate();

	DebugGUI();
}

void Player::Attack()
{
	if (bulletTimer_ > 0)
	{
		bulletTimer_ -= Timer::GetInstance()->GetRawDeltaTime();
	}
	if ((input_->PushKey(DIK_SPACE) || input_->IsPressMouse(0)) && bulletTimer_ <= 0)
	{
		currentBulletType_ = PlayerBulletType::NORMAL;
		Vector3 direction = Normalize(reticle_->GetTarget() - GetWorldPosition());
		bulletManager_->Fire(currentBulletType_, GetWorldPosition(), direction);
		GameAudio::GetInstance()->Play("attack", false, SoundCategory::SE);
		bulletTimer_ = bulletCoolTime_;
	}
}

void Player::Move()
{
	inputDir_ = {};
	roll = 0.0f;
	movePitch = 0.0f;

	if (input_->PushKey(DIK_W)) { inputDir_.y += 1.0f; movePitch = -0.1f; }
	if (input_->PushKey(DIK_S)) { inputDir_.y -= 1.0f; movePitch = 0.1f; }
	if (input_->PushKey(DIK_A)) { inputDir_.x -= 1.0f; roll = 0.1f; }
	if (input_->PushKey(DIK_D)) { inputDir_.x += 1.0f; roll = -0.1f; }

	if (Length(inputDir_) != 0.0f)
	{
		inputDir_ = Normalize(inputDir_);
		speed_.x = defaultSpeed_;
		speed_.y = defaultSpeed_ * (xRange / yRange);
		screenOffset_ += inputDir_ * speed_ * deltaTime_;
	}
	ClampOffset();

	// ===== ここが一番の変更点：ローカル座標で直接指定 =====
	// 親はカメラなので、+Z がカメラ前方（エンジンによっては -Z）になる
	worldTransform_.translation_ = {
		screenOffset_.x * xRange,
		screenOffset_.y * yRange,
		playerDepthFromCamera_      // 画面からの奥行き(カメラ前方)
	};

	RotationOffsetLocal();  // ←下の関数に差し替え
}

void Player::ClampOffset()
{
	screenOffset_.x = std::clamp(screenOffset_.x, -1.0f, 1.0f);
	screenOffset_.y = std::clamp(screenOffset_.y, -1.0f, 1.0f);
}

void Player::RotationOffsetLocal()
{
	// ローカル軸回りに回す（親＝カメラが最終的な向きを与えてくれる）
	// ロール：ローカル前方(Z)／ピッチ：ローカル右(X)
	Quaternion qRoll = MakeRotateAxisAngleQuaternion({ 0,0,1 }, roll);
	Quaternion qPitch = MakeRotateAxisAngleQuaternion({ 1,0,0 }, movePitch);

	// 好みで順序調整（ここでは Roll→Pitch）
	worldTransform_.rotationQ_ = Multiply(qRoll, qPitch);
	worldTransform_.TransferMatrix(); // 中で parent * local になること
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

void Player::DebugGUI()
{
#ifdef _DEBUG
	ImGui::Begin("Player");

	ImGui::Text("hp : %d", hitpoint_);

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
	reticleWT_.colliderScale_ = { colliderScale_, colliderScale_, colliderScale_ };
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
