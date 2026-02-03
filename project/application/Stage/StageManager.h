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
    void AddStage();
    void RemoveStage(size_t index);
    void DuplicateStage(size_t index);
    void SelectStage(size_t index);
    Stage* GetCurrentStage();

    void SaveStageToFile(size_t index, const std::string& path);
    void LoadStageFromFile(const std::string& path);

    void DrawEditorUI(); // ImGui編集UI

private:
    Camera* camera_ = nullptr;
    std::vector<std::unique_ptr<Stage>> stages_;
    size_t currentStageIndex_ = 0;
};

