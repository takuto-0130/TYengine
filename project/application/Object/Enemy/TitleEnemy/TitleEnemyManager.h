#pragma once
#include "../Enemy.h"
#include <random>

class Camera;

class TitleEnemyManager
{
public:
	void Init(Camera* camera);

	void Reset();

	void Update();

	void Draw();

	void Pop();

public:
	void SetTargetPos(Vector3* pos);

private:
	Vector3 ConvertScreenOffsetToWorld(const Vector2& offset);
	
private:
	Camera* camera_ = nullptr;

	std::list<std::unique_ptr<Enemy>> enemies_;

	float enemyPopDepthMin_ = 8.0f;
	float enemyPopDepthMax_ = 14.0f;
	float xRange = 16.0f * 0.09f * 2.0f; // 横移動の最大幅（画面内の物理スケール）
	float yRange = 9.0f * 0.085f * 2.0f; // 縦移動の最大高さ

	std::random_device rd;

	float timer_ = 0.0f;
	const float spawnReadyTimer_ = 2.0f;

	const int spawnNum_ = 3;
};

