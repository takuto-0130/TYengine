#pragma once

#include "Audio/Audio.h"
#include "Object/Player/Player.h"
#include "Object/Enemy/EnemyManager.h"
#include "Object/Rail/RailManager.h"
#include "ParticleManager.h"
#include "../AppSystem/Combo/ComboManager.h"
#include "../AppSystem/Score/ScoreManager.h"
#include <memory>
#include <list>

class Stage
{
public:
	void Init();

	void Reset();

	void Update();

	void Draw();

	void EditUpdate();

	nlohmann::json ToJson() const;

	void FromJson(const nlohmann::json& j);

	void SetCamera(Camera* camera) { camera_ = camera; }

	std::list<std::unique_ptr<Enemy>>& GetActiveEnemies() { return enemyManager_->GetActiveEnemies(); }

	bool EndRail() { return railManager_->IsEndRail(); }


	std::unique_ptr<Stage> Clone() const {
		auto clone = std::make_unique<Stage>();
		clone->SetCamera(camera_);
		clone->Init();

		// 現在のステージ内容をJSONでエクスポート → インポート
		nlohmann::json j = this->ToJson();
		clone->FromJson(j);

		return clone;
	}

public:
	ComboManager* GetComboManager() { return comboManager_.get(); }
	ScoreManager* GetScoreManager() { return scoreManager_.get(); }

private:
	Camera* camera_ = nullptr;
	std::unique_ptr<Player> player_;
	std::unique_ptr<EnemyManager> enemyManager_;
	std::unique_ptr<RailManager> railManager_;


	std::unique_ptr<ComboManager> comboManager_;
	std::unique_ptr<ScoreManager> scoreManager_;

	std::unique_ptr<Object3d> ground_;
	WorldTransform groundWT_;

	bool isEdit_ = false;
};

