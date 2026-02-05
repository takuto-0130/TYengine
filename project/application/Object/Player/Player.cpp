#include "Player.h"
#include "ColliderManager.h"
#include "Input.h"
#include "Timer.h"
#include "Quaternion.h"
#include "Effect/PlaneParticle.h"
#include "Effect/ContrailBehaviour.h"
#include "Effect/ParticleManager.h"
#include "../../AppSystem/Audio/GameAudio.h"

using namespace TYEngine::Utility;
using namespace TYEngine::Graphics;
using namespace TYEngine::Effect;
using namespace TYEngine;

#define PLAYER_STATE_ENTRY(stateEnum, funcName) \
    STATE_ENTRY_FOR(Player, stateEnum, funcName)

const std::vector<StateMachine<Player, PlayerState>::StateFunctionSet>& Player::GetStateTable()
{
	using enum PlayerState;
	static const std::vector<StateFunctionSet> stateTable = 
	{
		PLAYER_STATE_ENTRY(IDLE, Idle),
		PLAYER_STATE_ENTRY(ROUTE, Route),
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
	// ステートマシンの初期化
	RegisterFromDefaultTable(this);
	
	// 入力マネージャ取得
	input_ = Framework::Input::GetInstance();

	// 数値を適用
	colliderScale_ = 0.2f;
	playerDepthFromCamera_ = 4.0f;
	xRange = 16.0f * 0.09f;
	yRange = 9.0f * 0.085f;
	defaultSpeed_ = 0.3f;
	speed_ = { defaultSpeed_, defaultSpeed_ * (yRange / xRange) };

	// HP設定
	hitPoint_ = 9;

	// バレルロール
	rollTime_ = 0.6f;
	rollRange_ = 0.3f;
	leftRoll_ = 2.0f * std::numbers::pi_v<float>;
	rightRoll_ = -2.0f * std::numbers::pi_v<float>;
	justScale_ = 3.0f;
	bulletCoolTime_ = 0.1f;

	
	// 3Dオブジェクトの生成と初期化
	obj_ = std::make_unique<Object3d>();
	obj_->Initialize();
	obj_->SetModel("jett.obj");
	obj_->SetIsLighting(false);
	obj_->SetColor({ 1.0f, 1.0f, 1.0f, 1.0f });

	// ワールドトランスフォームの設定
	worldTransform_.Initialize();
	worldTransform_.SetScale({ colliderScale_, colliderScale_, colliderScale_ });
	worldTransform_.SetUseQuaternion(true); // クォータニオンを使用
	worldTransform_.SetParentMatrix(&camera_->GetWorldMatrix()); // 親行列をカメラに設定

	// 初期位置設定（画面手前への配置）
	Vector3 pos = worldTransform_.GetTranslation();
	pos.z = 4.0f;
	worldTransform_.SetTranslation(pos);
	worldTransform_.Update();

	// 通常コライダーの生成と登録
	collider_ = std::make_unique<PlayerCollider>(
		static_cast<uint32_t>(ColliderTypeID::PLAYER), 
		GetWorldPosition(), 
		colliderScale_, 
		this
	);
	ColliderManager::GetInstance()->AddCollider(collider_.get());

	// ジャスト回避用コライダーの生成と登録
	justCollider_ = std::make_unique<JustCollider>(
		static_cast<uint32_t>(ColliderTypeID::JUST_AREA),
		GetWorldPosition(),
		justScale_,
		this
	);
	ColliderManager::GetInstance()->AddCollider(justCollider_.get());


	// 初期ステートをROOTに設定
	ChangeState(PlayerState::ROUTE);

	// 自弾マネージャの初期化
	bulletManager_ = std::make_unique<PlayerBulletManager>(this);
	bulletManager_->SetCamera(camera_);
	bulletManager_->Init();
	
	// レティクルの初期化
	reticle_ = std::make_unique<Reticle>(camera_);
	reticle_->Init();


	// デバッグ用レティクル初期化
	ReticleInit();
}

void Player::Update()
{
	deltaTime_ = Timer::GetInstance()->GetDeltaTime();

	// 色のリセット（被弾などで赤くなっている場合があるため）
	obj_->SetColor({ 1.0f, 1.0f, 1.0f, 1.0f });

	// ステートマシンの更新
	UpdateState(deltaTime_);

	// エンジン噴射パーティクルの生成
	// プレイヤー後方にパーティクルを発生させる
	Vector3 back = -Normalize(Vector3(worldTransform_.GetMatWorld().m[2][0], worldTransform_.GetMatWorld().m[2][1], worldTransform_.GetMatWorld().m[2][2]));
	IParticleRenderer::Emitter e;
	e.transform.translate = GetWorldPosition() + back * 0.3f;
	e.transform.scale = { 0.1f ,0.1f ,0.1f };
	e.velocity = back * 3.0f;
	e.count = 1;
	e.frequency = 0.01f; // 毎フレーム発生

	ParticleManager::GetInstance()->SetEmitter(3, e);
	ParticleManager::GetInstance()->TriggerEmit(3, true);

	// ステート更新後の追加処理（移動反映や攻撃など）
	PostStateUpdate();
}

void Player::Draw()
{
	if(!isDead_)
	{
		obj_->Draw(worldTransform_);
		bulletManager_->Draw();

		ReticleDraw();
	}
}


////////////////////// ちょっとおかしいので後に修正 //////////////////////
void Player::TakeDamage()
{
#ifdef _DEBUG
	// デバッグ時ダメージ処理（現在はなし）
#else
	// 通常ダメージ処理：HP減少
	--hitPoint_;
#endif // _DEBUG

	if (hitPoint_ > 0)
	{
		// 生存していれば被弾音声再生・被弾ステートへ遷移
		GameAudio::GetInstance()->Play("damageP", false, SoundCategory::SE);
		ChangeState(PlayerState::TAKE_DAMAGE);
	}
	else
	{
		// HP0なら死亡処理
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
	// 回転・位置の更新
	RotationOffsetLocal();
	
	// カメラシェイクの影響を適用（シェイク分だけ位置をずらす）
	if(camera_->ShakeActive()) worldTransform_.SetTranslation(worldTransform_.GetTranslation() - camera_->GetShake());
	
	// 行列更新
	worldTransform_.Update();
	
	// コライダー位置の同期
	collider_->Update(GetWorldPosition());
	justCollider_->Update(GetWorldPosition());

	// 攻撃処理
	Attack();
	
	// 弾マネージャの更新
	bulletManager_->Update();

	// レティクルの更新
	reticle_->Update();

	// デバッグ用レティクルの更新
	ReticleUpdate();

	DebugGUI();
}

void Player::Attack()
{
	// クールタイムの減算
	if (bulletTimer_ > 0)
	{
		bulletTimer_ -= Timer::GetInstance()->GetRawDeltaTime();
	}
	
	// 攻撃入力があり、クールタイムが解消されていれば発射
	if ((input_->PushKey(DIK_SPACE) || input_->IsPressMouse(0)) && bulletTimer_ <= 0)
	{
		currentBulletType_ = PlayerBulletType::NORMAL;
		
		// レティクル方向への発射ベクトル計算
		Vector3 direction = Normalize(reticle_->GetTarget() - GetWorldPosition());
		
		// 弾発射
		bulletManager_->Fire(currentBulletType_, GetWorldPosition(), direction);
		
		// 射撃音再生
		GameAudio::GetInstance()->Play("attack", false, SoundCategory::SE);
		
		// クールタイム設定
		bulletTimer_ = bulletCoolTime_;
	}
}

void Player::Move()
{
	inputDir_ = {};
	roll = 0.0f;
	movePitch = 0.0f;

	// 入力に応じた移動方向と姿勢傾きの設定
	if (input_->PushKey(DIK_W)) { inputDir_.y += 1.0f; movePitch = -0.1f; }
	if (input_->PushKey(DIK_S)) { inputDir_.y -= 1.0f; movePitch = 0.1f; }
	if (input_->PushKey(DIK_A)) { inputDir_.x -= 1.0f; roll = 0.1f; }
	if (input_->PushKey(DIK_D)) { inputDir_.x += 1.0f; roll = -0.1f; }

	// 移動入力がある場合
	if (Length(inputDir_) != 0.0f)
	{
		inputDir_ = Normalize(inputDir_);
		
		// 画面アスペクト比を考慮した移動速度補正
		speed_.x = defaultSpeed_;
		speed_.y = defaultSpeed_ * (xRange / yRange);
		
		// スクリーン上のオフセット座標を更新
		screenOffset_ += inputDir_ * speed_ * deltaTime_;
	}
	
	// 移動範囲の制限
	ClampOffset();

	// 座標反映（ローカル座標系）
	// 親はカメラなので、+Z がカメラ前方
	worldTransform_.SetTranslation({
		screenOffset_.x * xRange,	// 横位置
		screenOffset_.y * yRange,	// 縦位置
		playerDepthFromCamera_      // 画面からの奥行き(カメラ前方)
		});

	// 回転の更新
	RotationOffsetLocal();
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
	worldTransform_.SetRotateQuaternion(Multiply(qRoll, qPitch));
	worldTransform_.Update();
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

	ImGui::Text("hp : %d", hitPoint_);

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

void Player::ReticleInit()
{
#ifdef _DEBUG
	reticleObj_ = std::make_unique<Object3d>();
	reticleObj_->Initialize();
	reticleObj_->SetModel("cube.obj");
	reticleObj_->SetIsLighting(false);
	reticleObj_->SetColor({ 0,1,0,1 });
	reticleWT_.Initialize();
	reticleWT_.SetScale({ colliderScale_, colliderScale_, colliderScale_ });
	reticleWT_.Update();
#endif // _DEBUG
}

void Player::ReticleUpdate()
{
#ifdef _DEBUG
	reticleWT_.SetRotate(worldTransform_.GetRotate());
	reticleWT_.SetTranslation(reticle_->GetTarget());
	reticleWT_.Update();
#endif // _DEBUG
}

void Player::ReticleDraw()
{
#ifdef _DEBUG
	reticleObj_->Draw(reticleWT_);
#endif // _DEBUG
}
