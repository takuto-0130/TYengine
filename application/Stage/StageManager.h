#pragma once
#include <vector>
#include <memory>
#include <string>
#include "Stage.h"

#ifdef _DEBUG
#include "imgui.h"
#endif

class StageManager
{
public:
    void Init();
    void Update();
    void Draw();
    void EditUpdate();

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
    std::vector<std::unique_ptr<Stage>> stages_;
    size_t currentStageIndex_ = 0;
};

