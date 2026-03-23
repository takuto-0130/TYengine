#include "Player.h"
#include "ColliderManager.h"
#include "Input.h"
#include "Timer.h"
#include "Quaternion.h"
#include "Sprite.h"
#include "Effect/PlaneParticle.h"
#include "Effect/ContrailBehaviour.h"
#include "Effect/ParticleManager.h"
#include "PostEffectManager.h"
#include "../../AppSystem/Audio/GameAudio.h"
#include "../Enemy/EnemyManager/EnemyManager.h"

using namespace TYEngine;
using namespace Utility;
using namespace Graphics;
using namespace Effect;

#define PLAYER_STATE_ENTRY(stateEnum, funcName) \
    STATE_ENTRY_FOR(Player, stateEnum, funcName)

const std::vector<Player::StateFunctionSet>& Player::GetStateTable()
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
	auto* pem = OffScreen::PostEffectManager::GetInstance();
	pem->SetEffectEnabled("RadialBlur", false);
	pem->SetEffectEnabled("Vignette", false);
}

void Player::Init()
{
	// ステートマシンの初期化
	stateMachine_.RegisterFromDefaultTable(this);
	
	// 入力マネージャ取得
	input_ = Framework::Input::GetInstance();

	JMInit();

	// 自弾マネージャの初期化
	bullets_.bulletManager = std::make_unique<PlayerBulletManager>(this);
	bullets_.bulletManager->SetCamera(camera_);
	bullets_.bulletManager->SetJM(&jm_);
	bullets_.bulletManager->Init();

	// 3Dオブジェクトの生成と初期化
	obj_ = std::make_unique<Object3d>();
	obj_->Initialize();
	obj_->SetModel("elementary_air_plane.obj");
	obj_->SetIsLighting(true);
	obj_->SetColor({ 1.0f, 1.0f, 1.0f, 1.0f });

	// ワールドトランスフォームの設定
	worldTransform_.Initialize();
	worldTransform_.SetScale(colliderScale_);
	worldTransform_.SetUseQuaternion(true); // クォータニオンを使用
	worldTransform_.SetParentMatrix(&camera_->GetWorldMatrix()); // 親行列をカメラに設定
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
		barrelRoll_.justScale,
		this
	);
	ColliderManager::GetInstance()->AddCollider(justCollider_.get());

	// レティクルの初期化
	reticle_ = std::make_unique<Reticle>(camera_);
	reticle_->Init();

	// 初期ステートをROOTに設定
	stateMachine_.ChangeState(PlayerState::ROUTE);
}

void Player::Update()
{
	deltaTime_ = Timer::GetInstance()->GetDeltaTime();

#ifdef _DEBUG
	ImGui::Begin("Player State Debug");
	if (ImGui::Button("Reset"))
	{
		Reset();
	}
	stateMachine_.DebugImGui("Player");
	ImGui::End();
#endif // _DEBUG

	DebugUpdate();

	// 色のリセット
	if(obj_->GetColor() != Vector4{ 1.0f, 1.0f, 1.0f, 1.0f }) obj_->SetColor({ 1.0f, 1.0f, 1.0f, 1.0f });

	// ステートマシンの更新
	stateMachine_.UpdateState(deltaTime_);

	ParticleUpdate();

	status_.hpSprBG->Update();
	if (status_.hitPoint >= 0)
	{
		status_.hpSpr->SetScale({ static_cast<float>(status_.hitPoint) / static_cast<float>(status_.maxHitPoint),1.0f });
	}
	status_.hpSpr->Update();

	// ステート更新後の追加処理（移動反映や攻撃など）
	PostStateUpdate();
}

void Player::ParticleUpdate()
{
	// エンジン噴射パーティクルの生成
	// プレイヤー後方にパーティクルを発生させる
	Vector3 back = -Normalize(Vector3(worldTransform_.GetMatWorld().m[2][0], worldTransform_.GetMatWorld().m[2][1], worldTransform_.GetMatWorld().m[2][2]));
	IParticleRenderer::Emitter e;
	e.transform.translate = GetWorldPosition() + back * jetEffect_.offSet;
	e.transform.scale = jetEffect_.scale;
	e.velocity = back * jetEffect_.speed;
	e.count = jetEffect_.count;
	e.frequency = jetEffect_.frequency;

	ParticleManager::GetInstance()->SetEmitter(3, e);
	ParticleManager::GetInstance()->TriggerEmit(3, true);
}

void Player::Draw()
{
	if(!isDead_)
	{
		obj_->Draw(worldTransform_);
		bullets_.bulletManager->Draw();

		reticle_->Draw();
	}
}

void Player::DrawUI()
{
	if (!isDead_)
	{
		// ※画面解像度（ウィンドウサイズ）を取得
		float screenWidth = TYEngine::Core::WindowsApp::kClientWidth;
		float screenHeight = TYEngine::Core::WindowsApp::kClientHeight;

		int index = 0;

		for (Enemy* target : lockOn_.lockedEnemies)
		{
			// 1. ダングリングポインタ対策：敵がまだ生きているか確認
			if (lockOn_.enemyManager && lockOn_.enemyManager->IsValidEnemy(target))
			{
				if(lockOn_.enemyManager->IsActive(target))
				{
					Utility::Vector2 ndc;

					// 2. 敵のワールド座標をNDCに変換
					if (camera_->WorldToNDC(target->GetWorldPosition(), ndc))
					{
						// 3. NDC (-1.0 ~ 1.0) を スクリーン座標 (0 ~ Width/Height) に変換
						// X座標: -1.0 -> 0,  0.0 -> Width/2,  1.0 -> Width
						float screenX = (ndc.x + 1.0f) * 0.5f * screenWidth;

						// Y座標: -1.0 -> Height,  0.0 -> Height/2,  1.0 -> 0 (Y軸反転)
						float screenY = (1.0f - ndc.y) * 0.5f * screenHeight;

						// 4. 計算したスクリーン座標 (screenX, screenY) に 
						// ロックオン用の2Dスプライトを描画する
						lockOn_.lockOnSpr[index]->SetPosition({ screenX, screenY });
						lockOn_.lockOnSpr[index]->Update();
						lockOn_.lockOnSpr[index]->Draw();

					}
					index++;
				}
			}
		}
		if(isInGame_)
		{
			status_.hpSprBG->Draw();
			status_.hpSpr->Draw();
		}
	}
}


void Player::TakeDamage()
{
#ifdef _DEBUG
	// デバッグ時ダメージ処理（現在はなし）
	if (input_->TriggerKey(DIK_DOWNARROW))
	{
		--status_.hitPoint;
	}
#else
	// 通常ダメージ処理：HP減少
	if(isInGame_)
	{
		--status_.hitPoint;
	}
#endif // _DEBUG

	OnCollision();
}

void Player::OnCollision()
{
	if (status_.hitPoint > 0)
	{
		// 生存していれば被弾ステートへ遷移
		stateMachine_.ChangeState(PlayerState::TAKE_DAMAGE);
	}
	else
	{
		// HP0以下なら死亡処理
		if (stateMachine_.GetCurrentState() != PlayerState::DEAD)
		{
			stateMachine_.ChangeState(PlayerState::DEAD);
		}
	}
}


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
	bullets_.bulletManager->Update();

	// レティクルの更新
	reticle_->Update();

	DebugGUI();
}

void Player::Attack()
{
	// 死亡時は攻撃処理を行わない
	if (stateMachine_.GetCurrentState() == PlayerState::DEAD) return;

	// クールタイムの減算
	if (lockOn_.lockOnTimer > 0)
	{
		lockOn_.lockOnTimer -= Timer::GetInstance()->GetRawDeltaTime();
	}

	// 現在の射撃ボタン入力状態
	bool isPressing = input_->PushKey(DIK_LCONTROL) || input_->IsPressMouse(1);

	// ▼ ボタンを押し続けている間（ロックオン・サーチフェーズ）
	if (isPressing && lockOn_.lockOnTimer <= 0)
	{
		// まだ最大ロック数に達していない場合のみサーチ
		if (lockOn_.lockedEnemies.size() < lockOn_.maxLockCount && lockOn_.enemyManager)
		{
			// reticle_ の画面座標は screenOffset_ とほぼ同義なのでそれを利用
			Enemy* target = lockOn_.enemyManager->GetBestLockOnTarget(camera_, screenOffset_, lockOn_.lockOnRadius, lockOn_.lockedEnemies);

			if (target)
			{
				lockOn_.lockedEnemies.push_back(target);
				// TODO: ここで「カシュッ」というロックオン音を鳴らす
				GameAudio::GetInstance()->Play("enter", false, SoundCategory::SE);
				lockOn_.lockOnTimer = lockOn_.maxLockOnCool;
			}
		}
	} // ▼ ボタンを離した瞬間（一斉発射フェーズ）
	else if (lockOn_.wasPressingShot && !isPressing)
	{
		if (!lockOn_.lockedEnemies.empty())
		{
			bullets_.currentBulletType = PlayerBulletType::HOMING; // ホーミング弾タイプ

			// 散らすための角度計算用
			int bulletCount = 0;
			int totalBullets = static_cast<int>(lockOn_.lockedEnemies.size());

			for (Enemy* target : lockOn_.lockedEnemies)
			{
				// ロックオン中に敵が倒されてポインタが無効になっていないか安全確認
				if (lockOn_.enemyManager->IsValidEnemy(target))
				{
					// 初期方向を散らす
					// カメラの上方向と右方向を取得
					Vector3 up = camera_->GetUp();
					Vector3 right = camera_->GetRight();
					Vector3 forward = camera_->GetForward();

					// 弾のインデックスに応じて、左右・上に散らす角度を計算
					// 例：-1.0 ~ 1.0 の間で左右に散らす
					float spreadX = (totalBullets > 1) ? -lockOn_.spreadX + ((lockOn_.spreadX * 2.0f) * bulletCount / (totalBullets - 1)) : 0.0f;

					// 上方向にも少し山なりに飛ばす
					float spreadY = lockOn_.spreadY;

					// 初期方向ベクトルを合成（前方に進みつつ、上と左右に広がる）
					Vector3 initialDir = forward + (right * spreadX) + (up * spreadY);
					initialDir = Normalize(initialDir);

					// 発射
					bullets_.bulletManager->Fire(bullets_.currentBulletType, GetWorldPosition(), initialDir, target, lockOn_.enemyManager);

					bulletCount++;
					// 発射音
					GameAudio::GetInstance()->Play("attack", false, SoundCategory::SE);
				}
			}


			// ロックオンリストをクリア
			lockOn_.lockedEnemies.clear();
		}
	}

	// 次のフレームのために状態を保持
	lockOn_.wasPressingShot = isPressing;


	// クールタイムの減算
	if (bullets_.bulletTimer > 0)
	{
		bullets_.bulletTimer -= Timer::GetInstance()->GetRawDeltaTime();
	}
	if (!isPressing)
	{
		// 攻撃入力があり、クールタイムが解消されていれば発射
		if ((input_->PushKey(DIK_SPACE) || input_->IsPressMouse(0)) && bullets_.bulletTimer <= 0)
		{
			bullets_.currentBulletType = PlayerBulletType::NORMAL;

			// レティクル方向への発射ベクトル計算
			Vector3 direction = Normalize(reticle_->GetTarget() - GetWorldPosition());

			// 弾発射
			bullets_.bulletManager->Fire(bullets_.currentBulletType, GetWorldPosition(), direction);

			// 射撃音再生
			GameAudio::GetInstance()->Play("attack", false, SoundCategory::SE);

			// クールタイム設定
			bullets_.bulletTimer = bullets_.bulletCoolTime;
		}
	}

}

void Player::Move()
{
	// 死亡時は操作処理を行わない
	if (stateMachine_.GetCurrentState() == PlayerState::DEAD) return;

	movement_.inputDir = {};
	movement_.roll = 0.0f;
	movement_.movePitch = 0.0f;

	// 入力に応じた移動方向と姿勢傾きの設定
	if (input_->PushKey(DIK_W)) { movement_.inputDir.y += 1.0f; movement_.movePitch = -movement_.maxPitch; }
	if (input_->PushKey(DIK_S)) { movement_.inputDir.y -= 1.0f; movement_.movePitch = movement_.maxPitch; }
	if (input_->PushKey(DIK_A)) { movement_.inputDir.x -= 1.0f; movement_.roll = movement_.maxRoll; }
	if (input_->PushKey(DIK_D)) { movement_.inputDir.x += 1.0f; movement_.roll = -movement_.maxPitch; }

	// 移動入力がある場合
	if (Length(movement_.inputDir) != 0.0f)
	{
		movement_.inputDir = Normalize(movement_.inputDir);
		
		movement_.CalculateSpeed();
		
		// スクリーン上のオフセット座標を更新
		screenOffset_ += movement_.inputDir * movement_.speed * deltaTime_;
	}
	
	// 移動範囲の制限
	ClampOffset();

	// 座標反映（ローカル座標系）
	// 親はカメラなので、+Z がカメラ前方
	worldTransform_.SetTranslation({
		screenOffset_.x * movement_.xRange,	// 横位置
		screenOffset_.y * movement_.yRange,	// 縦位置
		movement_.playerDepthFromCamera      // 画面からの奥行き(カメラ前方)
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
	Quaternion qRoll = MakeRotateAxisAngleQuaternion({ 0,0,1 }, movement_.roll);
	Quaternion qPitch = MakeRotateAxisAngleQuaternion({ 1,0,0 }, movement_.movePitch);

	// 好みで順序調整（ここでは Roll→Pitch）
	worldTransform_.SetRotateQuaternion(Multiply(qPitch, qRoll));
	worldTransform_.Update();
}

Vector3 Player::ConvertScreenOffsetToWorld(const Vector2& offset)
{
	Vector3 camPos = camera_->GetPosition();
	Vector3 camForward = camera_->GetForward();
	Vector3 camRight = camera_->GetRight();
	Vector3 camUp = camera_->GetUp();

	return camPos
		+ camForward * movement_.playerDepthFromCamera
		+ camRight * (offset.x * movement_.xRange)
		+ camUp * (offset.y * movement_.yRange);
}

void Player::DebugUpdate()
{
	DebugJMApply();
#ifdef _DEBUG
	ImGui::Begin("PlayerConfig");
	static JsonImGuiEditor inspector(jm_);
	inspector.Draw(jm_.Root(), "PlayerConfig.json");
	if (ImGui::Button("PlayerConfigSave")) jm_.Save();
	ImGui::End();
#endif // _DEBUG
}

void Player::DebugGUI()
{
#ifdef _DEBUG
	ImGui::Begin("Player");

	ImGui::Text("hp : %d", status_.hitPoint);

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

void Player::Reset()
{
	// ステータス・生存フラグのリセット
	status_.HPReset();
	isDead_ = false;

	//座標・姿勢のオフセットを初期位置(中央)へ戻す
	screenOffset_ = { 0.0f, 0.0f };
	movement_.inputDir = { 0.0f, 0.0f };
	movement_.roll = 0.0f;
	movement_.movePitch = 0.0f;

	// バレルロール（回避）関連のリセット
	barrelRoll_.isJust = false;
	barrelRoll_.justRoll = false;
	barrelRoll_.rollEffectTimer = 0.0f;

	// 攻撃・ロックオン関連のリセット
	lockOn_.lockedEnemies.clear();
	lockOn_.lockOnTimer = 0.0f;
	lockOn_.wasPressingShot = false;
	bullets_.bulletTimer = 0.0f;
	
	if (bullets_.bulletManager) bullets_.bulletManager->Clear();

	// 3Dモデルの状態リセット
	if (obj_)
	{
		obj_->SetColor({ 1.0f, 1.0f, 1.0f, 1.0f });
	}

	// コライダーの状態更新
	collider_->Update(GetWorldPosition());
	justCollider_->Update(GetWorldPosition());

	// ステートマシンを初期ステート（ROUTE）に戻す
	stateMachine_.ChangeState(PlayerState::ROUTE);

	// 一度ポストアップデートを呼んで、ワールドトランスフォームなどを即座に初期値へ反映させる
	PostStateUpdate();
}
