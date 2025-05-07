#include "RailEditor.h"
#include <fstream>
#include <json.hpp>

using json = nlohmann::json;

RailEditor* RailEditor::Instance() {
    static RailEditor instance;
    return &instance;
}

RailEditor::RailEditor() = default;

void RailEditor::Save(const std::string& filename) {
    json j;
    for (size_t i = 0; i < controlPoints_.size(); ++i) {
        const auto& p = controlPoints_[i];
        json pointJson = {
            {"x", p.x},
            {"y", p.y},
            {"z", p.z}
        };
        if (i < railSegments_.size()) {
            pointJson["segmentSpeed"] = railSegments_[i].speed;
        }
        j["controlPoints"].push_back(pointJson);
    }
    std::ofstream file(filename);
    file << j.dump(4);
}

void RailEditor::Load(const std::string& filename) {
    std::ifstream file(filename);
    if (!file.is_open()) return;

    json j;
    file >> j;
    controlPoints_.clear();
    railSegments_.clear();

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
        railSegments_.push_back(seg);
    }
}

void RailEditor::DrawEditorUI() {
#ifdef _DEBUG
    ImGui::Begin("Rail Editor");

    for (size_t idx = 0; idx < controlPoints_.size(); ++idx) {
        Vector3& p = controlPoints_[idx];
        RailSegment& seg = railSegments_[idx];

        ImGui::PushID(static_cast<int>(idx));
        std::string label = "Point " + std::to_string(idx);
        ImGui::DragFloat3(label.c_str(), &p.x, 0.1f);
        ImGui::DragFloat("Speed", &seg.speed, 0.1f, 0.1f, 10.0f);

        if (ImGui::Button("Up") && idx > 0) {
            std::swap(controlPoints_[idx], controlPoints_[idx - 1]);
            std::swap(railSegments_[idx], railSegments_[idx - 1]);
        }
        ImGui::SameLine();
        if (ImGui::Button("Down") && idx < controlPoints_.size() - 1) {
            std::swap(controlPoints_[idx], controlPoints_[idx + 1]);
            std::swap(railSegments_[idx], railSegments_[idx + 1]);
        }
        ImGui::SameLine();
        if (ImGui::Button("Delete")) {
            controlPoints_.erase(controlPoints_.begin() + idx);
            railSegments_.erase(railSegments_.begin() + idx);
            ImGui::PopID();
            break;
        }
        ImGui::PopID();
    }

    if (ImGui::Button("Add Point")) {
        Vector3 newPoint = controlPoints_.empty() ? Vector3{ 0, 0, 0 } : controlPoints_.back();
        controlPoints_.push_back(newPoint);
        railSegments_.push_back({});
    }

    if (ImGui::Button("Save")) {
        Save("Resources/JSON/RailEditor.json");
    }
    ImGui::SameLine();
    if (ImGui::Button("Load")) {
        Load("Resources/JSON/RailEditor.json");
    }
    ImGui::SameLine();
    if (ImGui::Button("Preview")) {
        needsPreviewUpdate_ = true;
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
