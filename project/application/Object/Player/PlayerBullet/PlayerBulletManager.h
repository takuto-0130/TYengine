#pragma once
#include <list>
#include <memory>

#include "PlayerBulletType.h"
#include "struct.h"
#include "../../BaseBullet/BaseBullet.h"

#include "Camera.h"

class Player;
class PlayerBulletManager
{
public:
	PlayerBulletManager(Player* player);
	~PlayerBulletManager() = default;

	/// <summary>
	/// 初期化
	/// </summary>
	void Init();

	/// <summary>
	/// 更新
	/// </summary>
	void Update();

	/// <summary>
	/// 描画
	/// </summary>
	void Draw();

	/// <summary>
	/// 弾の発射処理
	/// </summary>
	/// <param name="type"> 発射する弾の種類 </param>
	/// <param name="pos"> 射出点の座標 </param>
	/// <param name="direction"> 射出される向き </param>
	void Fire(PlayerBulletType type, const Vector3& pos, const Vector3& direction);

	void SetCamera(Camera* camera) { camera_ = camera; }

private:
	Player* player_ = nullptr;
	std::list<std::unique_ptr<BaseBullet>> bullets_;

	Camera* camera_ = nullptr;
};

