#pragma once
#include <vector>
#include <string>
#include "struct.h"

#ifdef _DEBUG
#include "imgui.h"
#endif

struct RailSegment {
    float speed = 1.0f;
};

class RailEditor {
public:
    static RailEditor* Instance();

    void Save(const std::string& filename);
    void Load(const std::string& filename);
    void DrawEditorUI();

    const std::vector<Vector3>& GetControlPoints() const;
    const std::vector<RailSegment>& GetSegments() const;
    bool NeedsPreviewUpdate() const;
    void ResetPreviewFlag();

private:
    RailEditor();

    std::vector<Vector3> controlPoints_;
    std::vector<RailSegment> railSegments_;
    bool needsPreviewUpdate_ = false;
};
