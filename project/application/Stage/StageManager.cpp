#include "StageManager.h"
#include <fstream>
#include <nlohmann/json.hpp>

using json = nlohmann::json;

void StageManager::Init() 
{
    stages_.clear();
    AddStage(); // 初期ステージを1つ追加
    GetCurrentStage()->Init();
    
    // ロード後に状態をリセット
    GetCurrentStage()->Reset();
    
    // 依存関係などの解決のため1フレーム更新
    Update(); 
}

void StageManager::Update() 
{
    if (!stages_.empty()) 
    {
        GetCurrentStage()->Update();
    }
}

void StageManager::Draw() 
{
    if (!stages_.empty()) 
    {
        GetCurrentStage()->Draw();
    }
}

void StageManager::DrawUI()
{
    if (!stages_.empty())
    {
        GetCurrentStage()->DrawUI();
    }
}

void StageManager::AddStage()
{
    // 新規ステージの生成と初期化
    auto stage = std::make_unique<Stage>();
    stage->SetCamera(camera_);
    stage->Init();
    stages_.push_back(std::move(stage));
    // 追加したステージを選択状態に
    currentStageIndex_ = stages_.size() - 1;
}

Stage* StageManager::GetCurrentStage() 
{
    if (stages_.empty()) return nullptr;
    return stages_[currentStageIndex_].get();
}