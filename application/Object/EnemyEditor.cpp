#include "EnemyEditor.h"
#include <json.hpp>

using json = nlohmann::json;

EnemyEditor::EnemyEditor(std::list<std::list<std::unique_ptr<Enemy>>>* enemies)
    : enemies_(enemies) {
    Load("Resources/JSON/EnemyEditor.json");
}

void EnemyEditor::AddGroup() {
    enemies_->emplace_back();
}

void EnemyEditor::RemoveGroup(int index) {
    auto it = enemies_->begin();
    std::advance(it, index);
    enemies_->erase(it);
}

void EnemyEditor::AddEnemyToGroup(int groupIdx, const Vector3& pos) {
    auto groupIt = enemies_->begin();
    std::advance(groupIt, groupIdx);
    std::unique_ptr<Enemy> enemy = std::make_unique<Enemy>();
    enemy->Init();
    enemy->SetPos(pos);
    groupIt->push_back(std::move(enemy));
}

void EnemyEditor::RemoveEnemyFromGroup(int groupIdx, int enemyIdx) {
    auto groupIt = enemies_->begin();
    std::advance(groupIt, groupIdx);
    auto enemyIt = groupIt->begin();
    std::advance(enemyIt, enemyIdx);
    groupIt->erase(enemyIt);
}

void EnemyEditor::Save(const std::string& filename) {
    json j;
    for (const auto& group : *enemies_) {
        json groupJson = json::array();
        for (const auto& enemy : group) {
            Vector3 pos = enemy->GetWorldPosition();
            groupJson.push_back({ {"x", pos.x}, {"y", pos.y}, {"z", pos.z} });
        }
        j["groups"].push_back(groupJson);
    }
    std::ofstream file(filename);
    file << j.dump(4);
}

void EnemyEditor::Load(const std::string& filename) {
    std::ifstream file(filename);
    if (!file.is_open()) return;

    json j;
    file >> j;
    enemies_->clear();

    for (const auto& groupJson : j["groups"]) {
        std::list<std::unique_ptr<Enemy>> group;
        for (const auto& enemyJson : groupJson) {
            Vector3 pos{
                enemyJson["x"].get<float>(),
                enemyJson["y"].get<float>(),
                enemyJson["z"].get<float>()
            };
            std::unique_ptr<Enemy> enemy = std::make_unique<Enemy>();
            enemy->Init();
            enemy->SetPos(pos);
            group.push_back(std::move(enemy));
        }
        enemies_->push_back(std::move(group));
    }
}

void EnemyEditor::DrawEditorUI() {
#ifdef _DEBUG
    ImGui::Begin("Enemy Editor");

    if (ImGui::Button("Save")) {
        Save("Resources/JSON/EnemyEditor.json");
    }
    ImGui::SameLine();
    if (ImGui::Button("Load")) {
        Load("Resources/JSON/EnemyEditor.json");
    }

    int groupIdx = 0;
    for (auto groupIt = enemies_->begin(); groupIt != enemies_->end(); ++groupIt, ++groupIdx) {
        std::string label = "Group " + std::to_string(groupIdx);
        if (ImGui::CollapsingHeader(label.c_str(), ImGuiTreeNodeFlags_DefaultOpen)) {
            int enemyIdx = 0;
            for (auto& enemy : *groupIt) {
                Vector3 pos = enemy->GetWorldPosition();
                ImGui::PushID(("Group" + std::to_string(groupIdx) + "_Enemy" + std::to_string(enemyIdx)).c_str());
                if (ImGui::DragFloat3("Position", &pos.x, 0.1f)) {
                    enemy->SetPos(pos);
                }
                if (ImGui::Button("Delete Enemy")) {
                    RemoveEnemyFromGroup(groupIdx, enemyIdx);
                    ImGui::PopID();
                    break;
                }
                ImGui::PopID();
                ++enemyIdx;
            }
            if (ImGui::Button(("Add Enemy##" + std::to_string(groupIdx)).c_str())) {
                AddEnemyToGroup(groupIdx, { 0, 0, 0 });
            }
            ImGui::SameLine();
            if (ImGui::Button(("Delete Group##" + std::to_string(groupIdx)).c_str())) {
                RemoveGroup(groupIdx);
                break;
            }
        }
    }

    if (ImGui::Button("Add Group")) {
        AddGroup();
    }

    ImGui::End();
#endif
}

