#pragma once

#include "Audio.h"
#include "BeatAnalyzer.h"
#include "LevelObject.h"
#include "Object/Player/Player.h"
#include "Object/Enemy/EnemyManager/EnemyManager.h"
#include "Object/Rail/RailManager.h"
#include "../AppSystem/Combo/ComboManager.h"
#include "../AppSystem/Score/ScoreManager.h"
#include <memory>
#include <json.hpp>

class Stage
{
public:
	/// <summary>
	/// ステージ初期化処理。
	/// プレイヤー、敵、レール、コンボ・スコアマネージャの生成と初期設定を行う。
	/// </summary>
	void Init();

	/// <summary>
	/// ステージ状態のリセット。
	/// レール進行度などを初期状態に戻す。
	/// </summary>
	void Reset();

	/// <summary>
	/// ステージの毎フレーム更新処理。
	/// ゲームプレイ中のロジック（レール進行、敵発生、プレイヤー更新）を実行。
	/// </summary>
	void Update();

	/// <summary>
	/// ステージの描画処理。
	/// 背景、レール、敵、プレイヤーの描画コマンドを発行する。
	/// </summary>
	void Draw();

	/// <summary>
	/// ステージの描画処理。
	/// UIの描画コマンドを発行する。
	/// </summary>
	void DrawUI();

	/// <summary>
	/// レールエディタ用の更新処理。
	/// 編集モード有効時に呼び出される。
	/// </summary>
	void EditUpdate();

	/// <summary>
	/// 現在のステージ構成（レールデータ等）をJSON形式でシリアライズする。
	/// </summary>
	/// <returns>シリアライズされたJSONオブジェクト。</returns>
	nlohmann::json ToJson() const;

	/// <summary>
	/// JSONデータからステージ構成を復元する。
	/// </summary>
	/// <param name="j">インポートするJSONオブジェクト。</param>
	void FromJson(const nlohmann::json& j);

	/// <summary>
	/// ステージで使用するカメラを設定する。
	/// </summary>
	/// <param name="camera">カメラへのポインタ。</param>
	void SetCamera(TYEngine::CameraSystem::Camera* camera) { camera_ = camera; }

	/// <summary>
	/// レールが終端に到達したかどうかを判定する。
	/// </summary>
	/// <returns>終端なら true。</returns>
	bool EndRail() { return railManager_->IsEndRail(); }


	/// <summary>
	/// ステージインスタンスのディープコピーを作成する（プロトタイプパターン用）。
	/// </summary>
	/// <returns>複製されたStageインスタンス。</returns>
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

	Player* GetPlayer() { return player_.get(); }

private:
	void StageObjectBeatScale();

private:
	/// <summary>カメラへのポインタ（借用）。</summary>
	TYEngine::CameraSystem::Camera* camera_ = nullptr;
	
	/// <summary>プレイヤーオブジェクト。</summary>
	std::unique_ptr<Player> player_;
	
	/// <summary>敵（タイトルシーン用）管理マネージャ。</summary>
	EnemyManager enemyMgr_;
	
	/// <summary>レール移動管理マネージャ。</summary>
	std::unique_ptr<RailManager> railManager_;

	/// <summary>コンボ計測・管理用マネージャ。</summary>
	std::unique_ptr<ComboManager> comboManager_;
	
	/// <summary>スコア計測・管理用マネージャ。</summary>
	std::unique_ptr<ScoreManager> scoreManager_;

	/// <summary>背景（地面）オブジェクト。</summary>
	std::unique_ptr<TYEngine::Graphics::Object3d> ground_;

	std::vector<std::unique_ptr<TYEngine::Utility::LevelObject>> stageObject_;
	
	/// <summary>背景のワールド変換情報。</summary>
	TYEngine::Utility::WorldTransform groundWT_;

	/// <summary>レールエディタによる編集モードフラグ。</summary>
	bool isEdit_ = false;

	/// <summary>ビートアナライザー（演出用）。</summary>
	TYEngine::AudioSystem::BeatAnalyzer beatAnalyzer_;
};

