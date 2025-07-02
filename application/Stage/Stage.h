#pragma once

#include "Sprite.h"
#include "Object3d.h"
#include "WorldTransform.h"
#include <sstream>
#include "Audio/Audio.h"
#include "Object/Player/Player.h"
#include "Object/Enemy/EnemyManager.h"
#include "Object/Rail/RailManager.h"
#include "Skydome/Skydome.h"
#include "ParticleManager.h"
#include <memory>
#include <vector>
#include <list>
#include <unordered_set>

class Stage
{
public:
	void Init();

	void Update();

	void Draw();

	void EditUpdate();

private:
	std::unique_ptr<Player> player_;
	std::unique_ptr<EnemyManager> enemyManager_;
	std::unique_ptr<RailManager> railManager_;
	std::unique_ptr<Skydome> skydome_;
};

