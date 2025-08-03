#pragma once
#include "../../BaseBullet/BaseBullet.h"

class EnemyBulletManager
{
public:
	EnemyBulletManager() = default;
	~EnemyBulletManager() = default;

	void Init();
	void Update();
	void Draw();

	void ClearAll() { bullets_.clear(); }

public:
	void AddBullet(std::unique_ptr<BaseBullet> bullet) { bullets_.push_back(std::move(bullet)); }

private:
	std::vector<std::unique_ptr<BaseBullet>> bullets_;
};

