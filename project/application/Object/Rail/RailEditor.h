#pragma once
#include <vector>
#include <string>
#include "struct.h"
#include <json.hpp>

#ifdef _DEBUG
#include "imgui.h"
#endif

/// <summary>
/// レール区間の定義構造体。
/// 各区間の速度倍率やイベントフラグを保持する。
/// </summary>
struct RailSegment {
    float speed = 1.0f;         ///< 速度倍率
    bool triggerEvent = false;  ///< イベントトリガー
};

/// <summary>
/// レールエディタクラス（シングルトン）。
/// キャットムル・ロム・スプライン曲線の制御点編集機能を提供する。
/// </summary>
class RailEditor {
public:
    static RailEditor* Instance();

    /// <summary>
    /// JSONファイルからレールデータを読み込む。
    /// </summary>
    /// <param name="filename">ファイルパス。</param>
    void Load(const std::string& filename);

    /// <summary>
    /// ImGuiによるエディタUIを描画する。
    /// </summary>
    void DrawEditorUI();

    /// <summary>制御点リストを取得する。</summary>
    const std::vector<TYEngine::Utility::Vector3>& GetControlPoints() const;
    /// <summary>セグメント情報リストを取得する。</summary>
    const std::vector<RailSegment>& GetSegments() const;
    
    /// <summary>プレビュー更新が必要かどうかを取得する。</summary>
    bool NeedsPreviewUpdate() const;
    /// <summary>プレビュー更新フラグをリセットする。</summary>
    void ResetPreviewFlag();

    nlohmann::json ToJson() const;
    void FromJson(const nlohmann::json& j);

private:
    RailEditor();

    std::vector<TYEngine::Utility::Vector3> controlPoints_;
    std::vector<RailSegment> railSegments_;
    bool needsPreviewUpdate_ = false;
};
