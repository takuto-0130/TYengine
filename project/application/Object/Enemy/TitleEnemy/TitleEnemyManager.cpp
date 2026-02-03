#include "TitleEnemyManager.h"
#include "Timer.h"


void EnemyManager::Init(Camera* camera)
{
	camera_ = camera;
	bulletManager_.Init();
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
	enemies_.remove_if([](const std::unique_ptr<Enemy>& e) { return e->IsDead(); });

	for (auto& enemy : enemies_)
	{
		enemy->Update();
	}

	timer_ += Timer::GetInstance()->GetDeltaTime();
	if (timer_ > spawnReadyTimer_)
	{
		timer_ = 0.0f;
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

	std::mt19937 gen(rd());
	std::uniform_real_distribution<float> dist(-1.0f, 1.0f);

	enemy->SetScreenPos({ dist(gen), dist(gen) });
	enemy->SetAndApplyPos(ConvertScreenOffsetToWorld(enemy->GetScreenPos()));
	enemy->Pop();
	enemy->SetEnemyBulletManager(&bulletManager_);
	enemy->SetEventListener(comboAndScoreHandler_.get());
	enemy->SetIsInGame(true);
	enemies_.push_back(std::move(enemy));
}

void EnemyManager::SetCamera(Camera* camera)
{
	for (auto& enemy : enemies_)
	{
		enemy->SetCamera(camera);
	}
}
