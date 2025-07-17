#pragma once
#include "Enemy.h"
#include "EnemyEditor.h"
#include "../../AppSystem/EventListener/EnemyEvent/ComboAndScoreHandler.h"
#include "json.hpp"

class EnemyManager
{
public:
	void Init();

	void MakeComboAndScoreHandler(ComboManager* combo, ScoreManager* score);

	void Reset();

	void Update();

	void Draw();

	void TriggerNextEnemyGroup();

	void UpdateEditorEnemies();
	void DrawEditorEnemies();
	void DrawEditorUI();

public:
	std::list<std::unique_ptr<Enemy>>& GetActiveEnemies() { return activeEnemies_; }

	EnemyEditor* GetEditor() const { return enemyEditor_.get(); }
private:
	std::list<std::list<std::unique_ptr<Enemy>>> DeepCopyEnemyGroups(const std::list<std::list<std::unique_ptr<Enemy>>>& src);

private:
	std::list<std::list<std::unique_ptr<Enemy>>> enemyGroupsEditor_;	// 編集用
	std::list<std::list<std::unique_ptr<Enemy>>> enemyGroups_;			// 全グループ（未出現）
	std::list<std::unique_ptr<Enemy>> activeEnemies_;					// 今出現中の敵
	std::unique_ptr<EnemyEditor> enemyEditor_;							// 敵のエディタ

	std::unique_ptr<ComboAndScoreHandler> comboAndScoreHandler_;
};

