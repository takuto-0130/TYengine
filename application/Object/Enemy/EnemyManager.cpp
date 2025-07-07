#include "EnemyManager.h"
#include "Enemy.h"
#include "EnemyEditor.h"

//----------------------------------
// 通常シーン
//----------------------------------
void EnemyManager::Init()
{
	enemyEditor_ = std::make_unique<EnemyEditor>(&enemyGroupsEditor_);
	enemyGroups_ = DeepCopyEnemyGroups(enemyGroupsEditor_);
}

void EnemyManager::Update()
{
	activeEnemies_.remove_if([](const std::unique_ptr<Enemy>& e) { return e->IsDead(); });

	for (auto& enemy : activeEnemies_) 
	{
		enemy->Update();
	}
}

void EnemyManager::Draw()
{
	for (auto& enemy : activeEnemies_) 
	{
		enemy->Draw();
	}
}

void EnemyManager::TriggerNextEnemyGroup()
{
	if (!enemyGroups_.empty()) {
		std::list<std::unique_ptr<Enemy>>& nextGroup = enemyGroups_.front();
		for (auto& enemy : nextGroup) {
			enemy->Pop();
			activeEnemies_.push_back(std::move(enemy));
		}
		enemyGroups_.pop_front();
	}
}


//----------------------------------
// エディタ系
//----------------------------------
void EnemyManager::UpdateEditorEnemies()
{
	for (const auto& group : enemyGroupsEditor_)
	{
		for (const auto& enemy : group)
		{
			enemy->UpdateTransform();
		}
	}
}

void EnemyManager::DrawEditorEnemies()
{
	for (const auto& group : enemyGroupsEditor_)
	{
		for (const auto& enemy : group)
		{
			enemy->Draw();
		}
	}
}

void EnemyManager::DrawEditorUI()
{
	enemyEditor_->DrawEditorUI();
}

std::list<std::list<std::unique_ptr<Enemy>>> EnemyManager::DeepCopyEnemyGroups(const std::list<std::list<std::unique_ptr<Enemy>>>& src) {
	std::list<std::list<std::unique_ptr<Enemy>>> copy;

	for (const auto& group : src) 
	{
		std::list<std::unique_ptr<Enemy>> newGroup;
		for (const auto& enemy : group) 
		{
			std::unique_ptr<Enemy> newEnemy = std::make_unique<Enemy>();
			newEnemy->Init();
			newEnemy->SetAndApplyPos(enemy->GetWorldPosition());
			newGroup.push_back(std::move(newEnemy));
		}
		copy.push_back(std::move(newGroup));
	}

	return copy;
}