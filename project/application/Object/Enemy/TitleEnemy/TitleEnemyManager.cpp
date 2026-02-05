#include "TitleEnemyManager.h"
#include "Timer.h"

using namespace TYEngine::Utility;
using namespace TYEngine;

void EnemyManager::Init(CameraSystem::Camera* camera)
{
	camera_ = camera;
	bulletManager_.Init();
	enemyPopDepthMin_ = 12.0f;
	enemyPopDepthMax_ = 17.0f;
	xRange = 16.0f * 0.09f * 2.0f;
	yRange = 9.0f * 0.085f * 2.0f;
	spawnReadyTimer_ = 2.0f;
	spawnNum_ = 5;
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
