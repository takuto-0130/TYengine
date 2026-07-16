#pragma once
#include <vector>
#include <memory>
#include <string>
#include "Stage.h"

#ifdef _DEBUG
#include "imgui.h"
#endif
class Camera;
/// <summary>
/// ステージ管理クラス。
/// 複数のステージデータの編集・切り替え・保存・読み込みを行う。
/// </summary>
class StageManager
{
public:
    StageManager(TYEngine::CameraSystem::Camera* camera) : camera_(camera) {};
    
    /// <summary>初期化処理。</summary>
    void Init();
    /// <summary>更新処理。</summary>
    void Update();
    /// <summary>描画処理。</summary>
    void Draw();
    /// <summary>UI描画処理。</summary>
    void DrawUI();

    /// <summary>現在のステージをリセットする。</summary>
    void Reset(){ GetCurrentStage()->Reset(); }

public:
    /// <summary>カメラを設定する。</summary>
    void SetCamera(TYEngine::CameraSystem::Camera* camera) { camera_ = camera; }
    
    /// <summary>
    /// レール終端に到達したか判定する。
    /// </summary>
    /// <returns>終端なら true。</returns>
    bool EndRail() { return GetCurrentStage()->EndRail(); }

    /// <summary>
    /// ステージ終了（クリア）条件を満たしているかを判定する。
    /// </summary>
    /// <returns>終了なら true。</returns>
    bool IsFinished() { return GetCurrentStage()->IsFinished(); }

    HitStreakManager* GetComboManager() 
    {
        return GetCurrentStage() ? GetCurrentStage()->GetComboManager() : nullptr;
    }

    ScoreManager* GetScoreManager()
    {
        return GetCurrentStage() ? GetCurrentStage()->GetScoreManager() : nullptr;
    }

    Player* GetPlayer() { return GetCurrentStage()->GetPlayer(); }

    RailManager* GetRailManager() { return GetCurrentStage()->GetRailManager(); }

    void SetBGMHandle(int handle) 
    { 
        GetPlayer()->SetBGMHandle(handle);
        GetRailManager()->SetBGMHandle(handle);
        GetCurrentStage()->SetBGMHandle(handle);
    }

private:
    /// <summary>新しいステージを追加する。</summary>
    void AddStage();
    /// <summary>現在選択中のステージを取得する。</summary>
    Stage* GetCurrentStage();

private:
    /// <summary>カメラ。</summary>
    TYEngine::CameraSystem::Camera* camera_ = nullptr;
    /// <summary>ステージリスト。</summary>
    std::vector<std::unique_ptr<Stage>> stages_;
    /// <summary>現在のステージインデックス。</summary>
    size_t currentStageIndex_ = 0;
};

