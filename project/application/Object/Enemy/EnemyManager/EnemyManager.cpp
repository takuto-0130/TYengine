#include "EnemyManager.h"
#include "Timer.h"

using namespace TYEngine::Utility;
using namespace TYEngine::AudioSystem;
using namespace TYEngine;

void EnemyManager::Init(CameraSystem::Camera* camera)
{
	camera_ = camera;
	bulletManager_.Init();
	enemyPopDepthMin_ = 12.0f;
	enemyPopDepthMax_ = 17.0f;
	xRange = 16.0f * 0.09f * 2.0f;
	yRange = 9.0f * 0.085f * 2.0f;
	spawnReadyTimer_ = 0.8f;
	spawnNum_ = 10;
}

void EnemyManager::Reset()
{
	enemies_.clear();
}

void EnemyManager::MakeComboAndScoreHandler(ComboManager* combo, ScoreManager* score)
{
	comboAndScoreHandler_ = std::make_unique<ComboAndScoreHandler>(combo, score);
}

void EnemyManager::Update()
{
	// 死亡した敵をリストから削除
	enemies_.remove_if([](const std::unique_ptr<Enemy>& e) { return e->IsDead(); });

	if (beatAnalyzer_)
	{
		static float timer = 0.0f;
		for (auto& enemy : enemies_)
		{
			if(enemy->GetCurrentState() == EnemyState::ACTIVE)
			{
				if (beatAnalyzer_->GetBeat())
				{
					timer = 0.0f;
					enemy->SetScale(enemy->GetUpScale());
				}
				else
				{
					if (timer < 1.0f)
					{
						timer += Timer::GetInstance()->GetDeltaTime();
					}
					enemy->SetScale(Lerp(enemy->GetUpScale(), enemy->GetDefaultScale(), timer));
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
		if (enemy->GetCurrentState() != EnemyState::ACTIVE) continue;

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

	return camPos
		+ camForward * 25.0f
		+ camRight * (offset.x * xRange)
		+ camUp * (offset.y * yRange);
}

void EnemyManager::Pop()
{
	std::unique_ptr<Enemy> enemy = std::make_unique<Enemy>();
	enemy->Init();

	// 画面座標系でのランダムな出現位置決定
	std::mt19937 gen(rd());
	std::uniform_real_distribution<float> dist(-1.0f, 1.0f);

	enemy->SetScreenPos({ dist(gen), dist(gen) });
	// スクリーン座標をワールド座標へ変換して設定
	enemy->SetAndApplyPos(ConvertScreenOffsetToWorld(enemy->GetScreenPos()));
	
	// 出現演出開始等
	enemy->Pop();
	
	// 依存関係の注入
	enemy->SetEnemyBulletManager(&bulletManager_);
	enemy->SetEventListener(comboAndScoreHandler_.get());
	enemy->SetIsInGame(true);
	
	enemies_.push_back(std::move(enemy));
}

void EnemyManager::SetCamera(CameraSystem::Camera* camera)
{
	for (auto& enemy : enemies_)
	{
		enemy->SetCamera(camera);
	}
}
