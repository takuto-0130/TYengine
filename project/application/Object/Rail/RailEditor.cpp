#include "RailEditor.h"
#include <fstream>

using json = nlohmann::json;

RailEditor* RailEditor::Instance() {
    static RailEditor instance;
    return &instance;
}

RailEditor::RailEditor() = default;

void RailEditor::Load(const std::string& filename) {
    std::ifstream file(filename);
    if (!file.is_open()) return;

    json j;
    file >> j;
    controlPoints_.clear();
    railSegments_.clear();

    // JSONから制御点データとセグメント情報を読み込み
    for (const auto& pointJson : j["controlPoints"]) {
        Vector3 p;
        p.x = pointJson["x"];
        p.y = pointJson["y"];
        p.z = pointJson["z"];
        controlPoints_.push_back(p);

        RailSegment seg;
        if (pointJson.contains("segmentSpeed")) {
            seg.speed = pointJson["segmentSpeed"];
        }
        if (pointJson.contains("triggerEvent")) {
            seg.triggerEvent = pointJson["triggerEvent"];
        }
        railSegments_.push_back(seg);
    }
}

void RailEditor::DrawEditorUI() {
#ifdef _DEBUG
    ImGui::Begin("Rail Editor");

    ImGui::SameLine();
    // プレビュー更新フラグを立てるボタン
    if (ImGui::Button("Preview")) {
        needsPreviewUpdate_ = true;
    }

    ImGui::Separator();

    // 各制御点の編集UI
    for (size_t idx = 0; idx < controlPoints_.size(); ++idx) {
        Vector3& p = controlPoints_[idx];
        RailSegment& seg = railSegments_[idx];

        std::string headerLabel = "Point " + std::to_string(idx);
        if (ImGui::CollapsingHeader(headerLabel.c_str())) {
            ImGui::PushID(static_cast<int>(idx));
            ImGui::DragFloat3("Position", &p.x, 0.1f);
            ImGui::DragFloat("Speed", &seg.speed, 0.1f, 0.1f, 10.0f);
            ImGui::Checkbox("Trigger Event", &seg.triggerEvent);

            // 順番入れ替え（上へ）
            if (ImGui::Button("Up") && idx > 0) {
                std::swap(controlPoints_[idx], controlPoints_[idx - 1]);
                std::swap(railSegments_[idx], railSegments_[idx - 1]);
            }
            ImGui::SameLine();
            // 順番入れ替え（下へ）
            if (ImGui::Button("Down") && idx < controlPoints_.size() - 1) {
                std::swap(controlPoints_[idx], controlPoints_[idx + 1]);
                std::swap(railSegments_[idx], railSegments_[idx + 1]);
            }
            ImGui::SameLine();
            // ポイント削除
            if (ImGui::Button("Delete")) {
                controlPoints_.erase(controlPoints_.begin() + idx);
                railSegments_.erase(railSegments_.begin() + idx);
                ImGui::PopID();
                break;
            }
            ImGui::PopID();
        }
    }

    // 新規ポイント追加
    if (ImGui::Button("Add Point")) {
        Vector3 newPoint = controlPoints_.empty() ? Vector3{ 0, 0, 0 } : controlPoints_.back();
        controlPoints_.push_back(newPoint);
        railSegments_.push_back({});
    }

    ImGui::End();
#endif
}

const std::vector<Vector3>& RailEditor::GetControlPoints() const {
    return controlPoints_;
}

const std::vector<RailSegment>& RailEditor::GetSegments() const {
    return railSegments_;
}

bool RailEditor::NeedsPreviewUpdate() const {
    return needsPreviewUpdate_;
}

void RailEditor::ResetPreviewFlag() {
    needsPreviewUpdate_ = false;
}

nlohmann::json RailEditor::ToJson() const {
    nlohmann::json j;
    for (size_t i = 0; i < controlPoints_.size(); ++i) {
        const auto& p = controlPoints_[i];
        json pointJson = {
            {"x", p.x},
            {"y", p.y},
            {"z", p.z}
        };
        if (i < railSegments_.size()) {
            pointJson["segmentSpeed"] = railSegments_[i].speed;
            pointJson["triggerEvent"] = railSegments_[i].triggerEvent;
        }
        j["controlPoints"].push_back(pointJson);
    }
    return j;
}

void RailEditor::FromJson(const nlohmann::json& j) {
    controlPoints_.clear();
    railSegments_.clear();
    for (const auto& pointJson : j["controlPoints"]) {
        Vector3 p = { pointJson["x"], pointJson["y"], pointJson["z"] };
        controlPoints_.push_back(p);
        RailSegment seg;
        seg.speed = pointJson.value("segmentSpeed", 1.0f);
        seg.triggerEvent = pointJson.value("triggerEvent", false);
        railSegments_.push_back(seg);
    }
}
