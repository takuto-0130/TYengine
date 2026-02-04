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
    StageManager(Camera* camera) : camera_(camera) {};
    
    /// <summary>初期化処理。</summary>
    void Init();
    /// <summary>更新処理。</summary>
    void Update();
    /// <summary>描画処理。</summary>
    void Draw();
    
    /// <summary>
    /// エディタ用更新処理。
    /// ImGuiによるステージ編集機能を提供する。
    /// </summary>
    void EditUpdate();

    /// <summary>現在のステージをリセットする。</summary>
    void Reset(){ GetCurrentStage()->Reset(); }

public:
    /// <summary>カメラを設定する。</summary>
    void SetCamera(Camera* camera) { camera_ = camera; }
    
    /// <summary>
    /// レール終端に到達したか判定する。
    /// </summary>
    /// <returns>終端なら true。</returns>
    bool EndRail() { return GetCurrentStage()->EndRail(); }

    ComboManager* GetComboManager() 
    {
        return GetCurrentStage() ? GetCurrentStage()->GetComboManager() : nullptr;
    }

    ScoreManager* GetScoreManager()
    {
        return GetCurrentStage() ? GetCurrentStage()->GetScoreManager() : nullptr;
    }

    Player* GetPlayer() { return GetCurrentStage()->GetPlayer(); }

private:
    /// <summary>新しいステージを追加する。</summary>
    void AddStage();
    /// <summary>指定したインデックスのステージを削除する。</summary>
    void RemoveStage(size_t index);
    /// <summary>指定したインデックスのステージを複製する。</summary>
    void DuplicateStage(size_t index);
    /// <summary>編集対象ステージを選択する。</summary>
    void SelectStage(size_t index);
    /// <summary>現在選択中のステージを取得する。</summary>
    Stage* GetCurrentStage();

    /// <summary>ステージを指定パスに保存する。</summary>
    void SaveStageToFile(size_t index, const std::string& path);
    /// <summary>指定パスからステージを読み込む。</summary>
    void LoadStageFromFile(const std::string& path);

    /// <summary>ImGuiによるステージ編集UI描画。</summary>
    void DrawEditorUI(); // ImGui編集UI

private:
    /// <summary>カメラ。</summary>
    Camera* camera_ = nullptr;
    /// <summary>ステージリスト。</summary>
    std::vector<std::unique_ptr<Stage>> stages_;
    /// <summary>現在のステージインデックス。</summary>
    size_t currentStageIndex_ = 0;
};

