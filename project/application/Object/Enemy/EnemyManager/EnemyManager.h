#pragma once
#include "../Enemy.h"
#include "../EnemyBullet/EnemyBulletManager.h"
#include "../../../AppSystem/EventListener/EnemyEvent/ComboAndScoreHandler.h"
#include "BeatAnalyzer.h"
#include <random>
#include <memory>
#define JSONMGR_WITH_IMGUI
#include "Utils/Json/JsonManager.h"

class Camera;

class RailManager;

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
	void MakeComboAndScoreHandler(HitStreakManager* combo, ScoreManager* score);

	/// <summary>状態をリセットする。</summary>
	void Reset();

	/// <summary>更新処理。</summary>
	void Update();

	/// <summary>描画処理。</summary>
	void Draw();

	/// <summary>UI描画処理。</summary>
	void DrawUI();

	/// <summary>敵をポップ（出現）させる。</summary>
	void Pop();

public:
	/// <summary>ターゲット（プレイヤー等）のワールド位置座標ポインタを設定する。</summary>
	/// <param name="pos">ワールド位置ベクトルポインタ。</param>
	void SetTargetPos(TYEngine::Utility::Vector3* pos);

	/// <summary>ビート解析クラスを設定する。</summary>
	/// <param name="beatAnalyzer">BeatAnalyzer ポインタ。</param>
	void SetBeatAnalyzer(TYEngine::AudioSystem::BeatAnalyzer* beatAnalyzer) { beatAnalyzer_ = beatAnalyzer; }

	/// <summary>ゲーム中フラグを設定する。</summary>
	/// <param name="is">ゲーム中なら true。</param>
	void SetIsInGame(bool is) { isInGame_ = is; }

	/// <summary>
	/// レティクルに最も近い未ロックオンの敵を取得する。
	/// </summary>
	/// <param name="camera">カメラポインタ。</param>
	/// <param name="reticleNDC">レティクルの NDC 座標。</param>
	/// <param name="lockRadiusNDC">ロックオン検索半径（NDC 単位）。</param>
	/// <param name="alreadyLockedEnemies">既にロックオン済みの敵リスト。</param>
	/// <returns>最適なターゲット敵のポインタ（無ければ nullptr）。</returns>
	Enemy* GetBestLockOnTarget(TYEngine::CameraSystem::Camera* camera, const TYEngine::Utility::Vector2& reticleNDC, float lockRadiusNDC, const std::vector<Enemy*>& alreadyLockedEnemies);

	/// <summary>
	/// 指定した敵ポインタが現在も生存・有効か確認する。
	/// </summary>
	/// <param name="enemyPtr">判定対象の敵ポインタ。</param>
	/// <returns>有効であれば true。</returns>
	bool IsValidEnemy(const Enemy* enemyPtr) const;

	/// <summary>
	/// 敵がアクティブ状態（攻撃・移動可能）かを判定する。
	/// </summary>
	/// <param name="enemyPtr">判定対象の敵ポインタ。</param>
	/// <returns>アクティブなら true。</returns>
	bool IsActive(Enemy* enemyPtr) const
	{
		if (enemyPtr->GetStateMachine().GetCurrentState() == EnemyState::ACTIVE)
		{
			return true;
		}
		return false;
	}

	/// <summary>敵の自然ポップ（スポーン）フラグを無効化する。</summary>
	void DisablePopFlag() { isPopFlag_ = false; }

	/// <summary>レール移動マネージャを設定する。</summary>
	/// <param name="railManager">RailManager ポインタ。</param>
	void SetRailManager(RailManager* railManager) { railManager_ = railManager; }

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

	HitStreakManager* hitStreakManager_ = nullptr;

	/// <summary>敵弾マネージャ。</summary>
	EnemyBulletManager bulletManager_;

	RailManager* railManager_ = nullptr;

	/// <summary>敵の出現深度（最小）。</summary>
	float enemyPopDepthMin_ = 0.0f;
	/// <summary>敵の出現深度（最大）。</summary>
	float enemyPopDepthMax_ = 0.0f;
	/// <summary>横移動の最大幅（出現範囲）。</summary>
	float xRange_ = 0.0f;
	/// <summary>縦移動の最大高さ（出現範囲）。</summary>
	float yRange_ = 0.0f;

	/// <summary>乱数生成器。</summary>
	std::random_device rd_;

	/// <summary>出現タイマー。</summary>
	float timer_ = 0.0f;
	/// <summary>出現間隔。</summary>
	float spawnReadyTimer_ = 0.0f;

	/// <summary>同時出現数。</summary>
	int spawnNum_ = 0;

	/// <summary>ゲームシーンかどうか。</summary>
	bool isInGame_ = false;

	/// <summary>ポップするかどうか。</summary>
	bool isPopFlag_ = true;

	/// <summary>コンボ・スコア処理ハンドラ。</summary>
	std::unique_ptr<ComboAndScoreHandler> comboAndScoreHandler_;

	TYEngine::AudioSystem::BeatAnalyzer* beatAnalyzer_;

	/// <summary>JSONマネージャ。</summary>
	std::unique_ptr<TYEngine::Utility::JsonManager> jsonManager_;

	// 配置パラメータ
	float minX_ = 0.5f;
	float maxX_ = 1.5f;
	float minHeightOffset_ = 2.0f;
	float maxHeightOffset_ = 7.0f;
	int comboStep_ = 20;
	float scaleInterpolationTime_ = 1.0f;
};

