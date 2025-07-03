#include "StageManager.h"
#include <fstream>
#include <nlohmann/json.hpp>

using json = nlohmann::json;

void StageManager::Init() {
    stages_.clear();
    AddStage(); // 初期ステージを1つ追加
    LoadStageFromFile("Resources/JSON/stage_data.json");
    GetCurrentStage()->Init();
    Update(); // 1フレーム更新
}

void StageManager::Update() {
    if (!stages_.empty()) {
        GetCurrentStage()->Update();
    }
}

void StageManager::Draw() {
    if (!stages_.empty()) {
        GetCurrentStage()->Draw();
    }
}

void StageManager::EditUpdate() {
    DrawEditorUI();
    if (!stages_.empty()) {
        GetCurrentStage()->EditUpdate();
    }
}

void StageManager::AddStage() {
    auto stage = std::make_unique<Stage>();
    stage->SetCamera(camera_);
    stage->Init();
    stages_.push_back(std::move(stage));
    currentStageIndex_ = stages_.size() - 1;
}

void StageManager::RemoveStage(size_t index) {
    if (index < stages_.size()) {
        stages_.erase(stages_.begin() + index);
        if (stages_.empty()) {
            AddStage();
        }
        currentStageIndex_ = std::min(index, stages_.size() - 1);
    }
}

void StageManager::DuplicateStage(size_t index) {
    if (index < stages_.size()) {
        auto clone = GetCurrentStage()->Clone();
        stages_.insert(stages_.begin() + index + 1, std::move(clone));
        currentStageIndex_ = index + 1;
    }
}

void StageManager::SelectStage(size_t index) {
    if (index < stages_.size()) {
        currentStageIndex_ = index;
    }
}

Stage* StageManager::GetCurrentStage() {
    if (stages_.empty()) return nullptr;
    return stages_[currentStageIndex_].get();
}

void StageManager::SaveStageToFile(size_t index, const std::string& path) {
    if (index < stages_.size()) {
        json j = stages_[index]->ToJson();
        std::ofstream ofs(path);
        ofs << j.dump(4);
    }
}

void StageManager::LoadStageFromFile(const std::string& path) {
    std::ifstream ifs(path);
    if (!ifs.is_open()) return;

    nlohmann::json j;
    ifs >> j;

    // 上書き対象ステージを取得
    if (stages_.empty()) {
        AddStage(); // 空の場合は追加
    }

    Stage* stage = GetCurrentStage();
    stage->FromJson(j);
}

void StageManager::DrawEditorUI() {
#ifdef _DEBUG
    ImGui::Begin("Stage Manager");

    for (int i = 0; i < static_cast<int>(stages_.size()); ++i) {
        ImGui::PushID(i);

        std::string label = "Stage " + std::to_string(i);
        bool selected = (i == static_cast<int>(currentStageIndex_));
        if (ImGui::Selectable(label.c_str(), selected)) {
            currentStageIndex_ = i;
        }

        if (ImGui::BeginDragDropSource()) {
            ImGui::SetDragDropPayload("STAGE_IDX", &i, sizeof(int));
            ImGui::Text("Move %s", label.c_str());
            ImGui::EndDragDropSource();
        }

        if (ImGui::BeginDragDropTarget()) {
            if (const ImGuiPayload* payload = ImGui::AcceptDragDropPayload("STAGE_IDX")) {
                int srcIdx = *(const int*)payload->Data;
                if (srcIdx != i) {
                    std::swap(stages_[srcIdx], stages_[i]);
                    if (currentStageIndex_ == srcIdx)
                        currentStageIndex_ = i;
                    else if (currentStageIndex_ == i)
                        currentStageIndex_ = srcIdx;
                }
            }
            ImGui::EndDragDropTarget();
        }

        ImGui::PopID();
    }

    ImGui::Separator();

    if (ImGui::Button("Add Stage")) AddStage();
    ImGui::SameLine();
    if (!stages_.empty() && ImGui::Button("Duplicate")) DuplicateStage(currentStageIndex_);
    ImGui::SameLine();
    if (!stages_.empty() && ImGui::Button("Delete")) RemoveStage(currentStageIndex_);

    static char path[128] = "Resources/JSON/stage_data.json";
    ImGui::InputText("Path", path, IM_ARRAYSIZE(path));

    if (ImGui::Button("Save Current Stage")) {
        SaveStageToFile(currentStageIndex_, path);
    }
    ImGui::SameLine();
    if (ImGui::Button("Load Stage")) {
        LoadStageFromFile(path);
    }

    ImGui::End();
#endif
}