#pragma once
#include <list>
#include <memory>

#include "PlayerBulletType.h"
#include "struct.h"
#include "../../BaseBullet/BaseBullet.h"

#include "Camera.h"
#include "Utils/Json/JsonManager.h"

class Player;
class Enemy;
class EnemyManager;
/// <summary>
/// プレイヤーの弾を一括管理するクラス。
/// 発射、更新、描画、衝突判定などを統括する。
/// </summary>
class PlayerBulletManager
{
public:
	PlayerBulletManager(Player* player);
	~PlayerBulletManager() = default;

	/// <summary>
	/// 初期化処理。
	/// </summary>
	void Init();

	/// <summary>
	/// 毎フレームの更新処理。
	/// 弾の移動、寿命削除などを行う。
	/// </summary>
	void Update();

	/// <summary>
	/// 全弾の描画処理。
	/// </summary>
	void Draw();

	/// <summary>
	/// 弾を発射する。
	/// </summary>
	/// <param name="type">弾の種類（NORMAL, MISSILEなど）。</param>
	/// <param name="pos">発射位置（ワールド座標）。</param>
	/// <param name="direction">発射方向（正規化ベクトル）。</param>
	void Fire(PlayerBulletType type, const TYEngine::Utility::Vector3& pos, const TYEngine::Utility::Vector3& direction, Enemy* target = nullptr, EnemyManager* mgr = nullptr);

	/// <summary>
	/// カメラを設定する（ビルボード用）。
	/// </summary>
	void SetCamera(TYEngine::CameraSystem::Camera* camera) { camera_ = camera; }

	void SetJM(TYEngine::Utility::JsonManager* jm) { jm_ = jm; }

private:
	/// <summary>所有者（プレイヤー）。</summary>
	Player* player_ = nullptr;
	/// <summary>弾リスト。</summary>
	std::list<std::unique_ptr<BaseBullet>> bullets_;

	/// <summary>カメラ。</summary>
	TYEngine::CameraSystem::Camera* camera_ = nullptr;

	/// <summary>JSONマネージャ。</summary>
	TYEngine::Utility::JsonManager* jm_ = nullptr;
};

