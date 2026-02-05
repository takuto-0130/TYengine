#pragma once
#include "../Enemy.h"
#include "../EnemyBullet/EnemyBulletManager.h"
#include "../../../AppSystem/EventListener/EnemyEvent/ComboAndScoreHandler.h"
#include <random>

class Camera;

/// <summary>
/// 敵管理クラス。
/// 敵キャラクターの生成と管理を行う。
/// </summary>
class EnemyManager
{
public:
	/// <summary>初期化処理。</summary>
	void Init(TYEngine::CameraSystem::Camera* camera);

	/// <summary>
	/// スコア・コンボ管理ハンドラを生成・登録する。
	/// </summary>
	void MakeComboAndScoreHandler(ComboManager* combo, ScoreManager* score);

	/// <summary>状態をリセットする。</summary>
	void Reset();

	/// <summary>更新処理。</summary>
	void Update();

	/// <summary>描画処理。</summary>
	void Draw();

	/// <summary>敵をポップ（出現）させる。</summary>
	void Pop();

public:
	void SetTargetPos(TYEngine::Utility::Vector3* pos);

	void SetCamera(TYEngine::CameraSystem::Camera* camera);

private:
	/// <summary>
	/// スクリーン座標オフセットをワールド座標に変換する。
	/// </summary>
	/// <param name="offset">スクリーンオフセット。</param>
	/// <returns>ワールド座標。</returns>
	TYEngine::Utility::Vector3 ConvertScreenOffsetToWorld(const TYEngine::Utility::Vector2& offset);
	
private:
	/// <summary>カメラへのポインタ。</summary>
	TYEngine::CameraSystem::Camera* camera_ = nullptr;

	/// <summary>管理中の敵リスト。</summary>
	std::list<std::unique_ptr<Enemy>> enemies_;

	/// <summary>敵弾マネージャ。</summary>
	EnemyBulletManager bulletManager_;

	/// <summary>敵の出現深度（最小）。</summary>
	float enemyPopDepthMin_ = 0.0f;
	/// <summary>敵の出現深度（最大）。</summary>
	float enemyPopDepthMax_ = 0.0f;
	/// <summary>横移動の最大幅（出現範囲）。</summary>
	float xRange = 0.0f;
	/// <summary>縦移動の最大高さ（出現範囲）。</summary>
	float yRange = 0.0f;

	/// <summary>乱数生成器。</summary>
	std::random_device rd;

	/// <summary>出現タイマー。</summary>
	float timer_ = 0.0f;
	/// <summary>出現間隔。</summary>
	float spawnReadyTimer_ = 0.0f;

	/// <summary>同時出現数。</summary>
	int spawnNum_ = 0;

	/// <summary>コンボ・スコア処理ハンドラ。</summary>
	std::unique_ptr<ComboAndScoreHandler> comboAndScoreHandler_;
};

