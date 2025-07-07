#pragma once
#include <vector>
#include <memory>
#include <string>
#include "Stage.h"

#ifdef _DEBUG
#include "imgui.h"
#endif
class Camera;
class StageManager
{
public:
    void Init();
    void Update();
    void Draw();
    void EditUpdate();

    void SetCamera(Camera* camera) { camera_ = camera; }

    std::list<std::unique_ptr<Enemy>>& GetActiveEnemies() { return GetCurrentStage()->GetActiveEnemies(); }

    bool EndRail() { return GetCurrentStage()->EndRail(); }

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

