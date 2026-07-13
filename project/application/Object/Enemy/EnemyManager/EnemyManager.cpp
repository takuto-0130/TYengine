#include "EnemyManager.h"
#include "Timer.h"
#include "../../Rail/RailManager.h"

using namespace TYEngine::Utility;
using namespace TYEngine::AudioSystem;
using namespace TYEngine;

void EnemyManager::Init(CameraSystem::Camera* camera)
{
	jsonManager_ = std::make_unique<JsonManager>();
	std::string err;
	jsonManager_->Load("EnemyConfig.json", true, &err);

	camera_ = camera;
	bulletManager_.Init();

	enemyPopDepthMin_ = jsonManager_->Get<float>("enemyManager.popDepthMin", 30.0f);
	enemyPopDepthMax_ = jsonManager_->Get<float>("enemyManager.popDepthMax", 40.0f);

	float xCoeff = jsonManager_->Get<float>("enemyManager.xRangeCoeff", 16.0f);
	float yCoeff = jsonManager_->Get<float>("enemyManager.yRangeCoeff", 9.0f);
	xRange = xCoeff * 0.09f * 2.0f;
	yRange = yCoeff * 0.085f * 2.0f;

	spawnReadyTimer_ = jsonManager_->Get<float>("enemyManager.spawnReadyTimer", 0.8f);
	spawnNum_ = jsonManager_->Get<int>("enemyManager.spawnNum", 15);

	minX_ = jsonManager_->Get<float>("enemyManager.minX", 0.5f);
	maxX_ = jsonManager_->Get<float>("enemyManager.maxX", 1.5f);
	minHeightOffset_ = jsonManager_->Get<float>("enemyManager.minHeightOffset", 2.0f);
	maxHeightOffset_ = jsonManager_->Get<float>("enemyManager.maxHeightOffset", 7.0f);
	comboStep_ = jsonManager_->Get<int>("enemyManager.comboStep", 20);
	scaleInterpolationTime_ = jsonManager_->Get<float>("enemyManager.scaleInterpolationTime", 1.0f);
}

void EnemyManager::Reset()
{
	enemies_.clear();
}

void EnemyManager::MakeComboAndScoreHandler(HitStreakManager* combo, ScoreManager* score)
{
	comboAndScoreHandler_ = std::make_unique<ComboAndScoreHandler>(combo, score);
	hitStreakManager_ = combo;
}

void EnemyManager::Update()
{
#ifdef _DEBUG
	enemyPopDepthMin_ = jsonManager_->Get<float>("enemyManager.popDepthMin", 30.0f);
	enemyPopDepthMax_ = jsonManager_->Get<float>("enemyManager.popDepthMax", 40.0f);
	float xCoeff = jsonManager_->Get<float>("enemyManager.xRangeCoeff", 16.0f);
	float yCoeff = jsonManager_->Get<float>("enemyManager.yRangeCoeff", 9.0f);
	xRange = xCoeff * 0.09f * 2.0f;
	yRange = yCoeff * 0.085f * 2.0f;
	spawnReadyTimer_ = jsonManager_->Get<float>("enemyManager.spawnReadyTimer", 0.8f);
	spawnNum_ = jsonManager_->Get<int>("enemyManager.spawnNum", 15);
	minX_ = jsonManager_->Get<float>("enemyManager.minX", 0.5f);
	maxX_ = jsonManager_->Get<float>("enemyManager.maxX", 1.5f);
	minHeightOffset_ = jsonManager_->Get<float>("enemyManager.minHeightOffset", 2.0f);
	maxHeightOffset_ = jsonManager_->Get<float>("enemyManager.maxHeightOffset", 7.0f);
	comboStep_ = jsonManager_->Get<int>("enemyManager.comboStep", 20);
	scaleInterpolationTime_ = jsonManager_->Get<float>("enemyManager.scaleInterpolationTime", 1.0f);
#endif

	// 死亡した敵をリストから削除
	enemies_.remove_if([](const std::unique_ptr<Enemy>& e) { return e->IsDead(); });

	if (beatAnalyzer_)
	{
		static float timer = 0.0f;
		for (auto& enemy : enemies_)
		{
			if(enemy->GetStateMachine().GetCurrentState() == EnemyState::ACTIVE)
			{
				if (beatAnalyzer_->GetBeat())
				{
					timer = 0.0f;
					enemy->SetScale(enemy->GetUpScale());
				}
				else
				{
					if (timer < scaleInterpolationTime_)
					{
						timer += Timer::GetInstance()->GetDeltaTime();
					}
					float t = scaleInterpolationTime_ > 0.0f ? (timer / scaleInterpolationTime_) : 1.0f;
					enemy->SetScale(Lerp(enemy->GetUpScale(), enemy->GetDefaultScale(), t));
				}
			}
		}
	}

	// 各敵の更新
	for (auto& enemy : enemies_)
	{
		enemy->Update();
	}

	// 定期的な敵のスポーン処理
	timer_ += Timer::GetInstance()->GetDeltaTime();
	if (timer_ > spawnReadyTimer_)
	{
		timer_ = 0.0f;
		// 上限数未満なら新規スポーン
		if (enemies_.size() < spawnNum_)
		{
			Pop();
		}
	}
	bulletManager_.Update();
}

void EnemyManager::Draw()
{
	for (auto& enemy : enemies_)
	{
		enemy->Draw();
	}
	bulletManager_.Draw();
}

void EnemyManager::DrawUI()
{
}

void EnemyManager::SetTargetPos(Vector3* pos)
{
	if (pos)
	{
		for (auto& enemy : enemies_)
		{
			enemy->SetTargetPos(*pos);
		}
	}
}

Enemy* EnemyManager::GetBestLockOnTarget(TYEngine::CameraSystem::Camera* camera, const Utility::Vector2& reticleNDC, float lockRadiusNDC, const std::vector<Enemy*>& alreadyLockedEnemies)
{
	Enemy* bestTarget = nullptr;
	float minDistanceSq = lockRadiusNDC * lockRadiusNDC; // ロックオン可能な最大距離の二乗

	for (auto& enemy : enemies_)
	{
		// アクティブ状態以外（出現中や死亡済）は除外
		if (enemy->GetStateMachine().GetCurrentState() != EnemyState::ACTIVE) continue;

		// 既にロックオン済みの敵は除外
		auto it = std::find(alreadyLockedEnemies.begin(), alreadyLockedEnemies.end(), enemy.get());
		if (it != alreadyLockedEnemies.end()) continue;

		Utility::Vector2 enemyNDC;
		if (camera->WorldToNDC(enemy->GetWorldPosition(), enemyNDC))
		{
			// レティクルとの距離（画面上の距離）を計算
			float dx = enemyNDC.x - reticleNDC.x;
			float dy = enemyNDC.y - reticleNDC.y;
			float distSq = dx * dx + dy * dy;

			if (distSq < minDistanceSq)
			{
				minDistanceSq = distSq;
				bestTarget = enemy.get();
			}
		}
	}
	return bestTarget;
}

bool EnemyManager::IsValidEnemy(const Enemy* enemyPtr) const
{
	// enemies_ リストの中にポインタがまだ存在するか確認（ダングリングポインタ対策）
	for (const auto& enemy : enemies_)
	{
		if (enemy.get() == enemyPtr)
		{
			return true;
		}
	}
	return false;
}

Vector3 EnemyManager::ConvertScreenOffsetToWorld(const Vector2& offset)
{
	Vector3 camPos = camera_->GetPosition();
	Vector3 camForward = camera_->GetForward();
	Vector3 camRight = camera_->GetRight();
	Vector3 camUp = camera_->GetUp();

	std::mt19937 gen(rd());
	std::uniform_real_distribution<float> dist(enemyPopDepthMin_, enemyPopDepthMax_);

	float depth = dist(gen);

	return camPos
		+ camForward * depth
		+ camRight * (offset.x * xRange)
		+ camUp * (offset.y * yRange);
}

void EnemyManager::Pop()
{
	if(hitStreakManager_)
	{
		for (int i = 0; i < 1 + (hitStreakManager_->GetComboCount() / comboStep_); i++)
		{
			if (isPopFlag_)
			{
				std::unique_ptr<Enemy> enemy = std::make_unique<Enemy>();
				enemy->Init();

				// 画面座標系でのランダムな出現位置決定
				std::mt19937 gen(rd());
				std::uniform_real_distribution<float> dist(-1.0f, 1.0f);

				// 元となる -1.0 ～ 1.0 の乱数を取得
				float rawX = dist(gen);
				float rawY = dist(gen);

				// 中心部を避けて左右に広げる処理
				float finalX = 0.0f;
				float minX = minX_;  // 中心部をどれだけ避けるか（0.0で中央、値を大きくするほど中央が空く）
				float maxX = maxX_;  // 左右にどれだけ広げるか（1.0より大きくすると画面外側まで広がる）

				if (rawX >= 0.0f)
				{
					// 右側にスポーン（minX ～ maxX の範囲に変換）
					finalX = minX + rawX * (maxX - minX);
				}
				else
				{
					// 左側にスポーン（-minX ～ -maxX の範囲に変換）
					// rawXは負の数なので、(maxX - minX) を掛けてマイナス方向に引き延ばす
					finalX = -minX + rawX * (maxX - minX);
				}

				// 補正したスクリーン座標を設定
				enemy->SetScreenPos({ finalX, rawY });

				// スクリーン座標をワールド座標へ変換
				Vector3 spawnPos = ConvertScreenOffsetToWorld(enemy->GetScreenPos());

				// --- 【ここから変更】地面基準でY座標をランダムな高さに設定 ---
				if (railManager_)
				{
					float terrainY = railManager_->GetTerrainHeight(spawnPos);

					// 地面からどれくらい浮かせるかのランダムな範囲
					float minHeightOffset = minHeightOffset_; // 最小の浮遊高度
					float maxHeightOffset = maxHeightOffset_; // 最大の浮遊高度

					// 新しく高さ用の乱数を生成
					std::uniform_real_distribution<float> heightDist(minHeightOffset, maxHeightOffset);
					float randomHeight = heightDist(gen);

					// ConvertScreenOffsetToWorldで計算したY座標を捨てて、地面基準の高さで上書きする
					spawnPos.y = terrainY + randomHeight;
				}
				// --- 【ここまで変更】 ---

				// 補正した座標を設定
				enemy->SetAndApplyPos(spawnPos);

				// 補正した座標を設定
				enemy->SetAndApplyPos(spawnPos);

				// 依存関係の注入
				enemy->SetEnemyBulletManager(&bulletManager_);
				enemy->SetEventListener(comboAndScoreHandler_.get());
				enemy->SetIsInGame(isInGame_);
				enemy->SetCamera(camera_);

				// 出現演出開始等
				enemy->Pop();

				enemies_.push_back(std::move(enemy));
			}
		}
	}
	else
	{
		if (isPopFlag_)
		{
			std::unique_ptr<Enemy> enemy = std::make_unique<Enemy>();
			enemy->Init();

			// 画面座標系でのランダムな出現位置決定
			std::mt19937 gen(rd());
			std::uniform_real_distribution<float> dist(-1.0f, 1.0f);

			// 元となる -1.0 ～ 1.0 の乱数を取得
			float rawX = dist(gen);
			float rawY = dist(gen);

			// 中心部を避けて左右に広げる処理
			float finalX = 0.0f;
			float minX = minX_;  // 中心部をどれだけ避けるか（0.0で中央、値を大きくするほど中央が空く）
			float maxX = maxX_;  // 左右にどれだけ広げるか（1.0より大きくすると画面外側まで広がる）

			if (rawX >= 0.0f)
			{
				// 右側にスポーン（minX ～ maxX の範囲に変換）
				finalX = minX + rawX * (maxX - minX);
			}
			else
			{
				// 左側にスポーン（-minX ～ -maxX の範囲に変換）
				// rawXは負の数なので、(maxX - minX) を掛けてマイナス方向に引き延ばす
				finalX = -minX + rawX * (maxX - minX);
			}

			// 補正したスクリーン座標を設定
			enemy->SetScreenPos({ finalX, rawY });

			// スクリーン座標をワールド座標へ変換
			Vector3 spawnPos = ConvertScreenOffsetToWorld(enemy->GetScreenPos());

			// --- 【ここから変更】地面基準でY座標をランダムな高さに設定 ---
			if (railManager_)
			{
				float terrainY = railManager_->GetTerrainHeight(spawnPos);

				// 地面からどれくらい浮かせるかのランダムな範囲
				float minHeightOffset = minHeightOffset_; // 最小の浮遊高度
				float maxHeightOffset = maxHeightOffset_; // 最大の浮遊高度

				// 新しく高さ用の乱数を生成
				std::uniform_real_distribution<float> heightDist(minHeightOffset, maxHeightOffset);
				float randomHeight = heightDist(gen);

				// ConvertScreenOffsetToWorldで計算したY座標を捨てて、地面基準の高さで上書きする
				spawnPos.y = terrainY + randomHeight;
			}
			// --- 【ここまで変更】 ---

			// 補正した座標を設定
			enemy->SetAndApplyPos(spawnPos);

			// 補正した座標を設定
			enemy->SetAndApplyPos(spawnPos);

			// 依存関係の注入
			enemy->SetEnemyBulletManager(&bulletManager_);
			enemy->SetEventListener(comboAndScoreHandler_.get());
			enemy->SetIsInGame(isInGame_);
			enemy->SetCamera(camera_);

			// 出現演出開始等
			enemy->Pop();

			enemies_.push_back(std::move(enemy));
		}
	}
}
