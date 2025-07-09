#pragma once
#include <list>
#include <memory>

class Player;
class BaseBullet;
class PlayerBulletManager
{
public:
	void Init(Player* player);
	void Update();
	void Draw();


private:
	Player* player_ = nullptr;
	std::list<std::unique_ptr<BaseBullet>> bullets_;
};

