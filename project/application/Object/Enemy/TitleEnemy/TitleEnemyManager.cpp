#include "TitleEnemyManager.h"
#include "Timer.h"


void TitleEnemyManager::Init(Camera* camera)
{
	camera_ = camera;
}

void TitleEnemyManager::Reset()
{
	enemies_.clear();
}

void TitleEnemyManager::Update()
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

}

void TitleEnemyManager::Draw()
{
	for (auto& enemy : enemies_)
	{
		enemy->Draw();
	}
}

void TitleEnemyManager::SetTargetPos(Vector3* pos)
{
	if (pos)
	{
		for (auto& enemy : enemies_)
		{
			enemy->SetTargetPos(*pos);
		}
	}
}

Vector3 TitleEnemyManager::ConvertScreenOffsetToWorld(const Vector2& offset)
{
	Vector3 camPos = camera_->GetPosition();
	Vector3 camForward = camera_->GetForward();
	Vector3 camRight = camera_->GetRight();
	Vector3 camUp = camera_->GetUp();

	std::mt19937 gen(rd());
	std::uniform_real_distribution<float> dist(enemyPopDepthMin_, enemyPopDepthMax_);

	return camPos
		+ camForward * dist(gen)
		+ camRight * (offset.x * xRange)
		+ camUp * (offset.y * yRange);
}

void TitleEnemyManager::Pop()
{
	std::unique_ptr<Enemy> enemy = std::make_unique<Enemy>();
	enemy->Init();

	std::mt19937 gen(rd());
	std::uniform_real_distribution<float> dist(-1.0f, 1.0f);

	enemy->SetAndApplyPos(ConvertScreenOffsetToWorld({ dist(gen), dist(gen) }));
	enemy->Pop();
	enemy->SetIsInGame(false);
	enemies_.push_back(std::move(enemy));
}
