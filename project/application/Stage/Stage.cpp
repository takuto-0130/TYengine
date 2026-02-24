#include "Stage.h"
#include "../Object/Rail/RailEditor.h"
#include "../AppSystem//Audio//GameAudio.h"

using namespace TYEngine::Utility;
using namespace TYEngine::Graphics;

void Stage::Init()
{
    beatAnalyzer_.Init("418", GameAudio::GetInstance()->CategoryToString(SoundCategory::BGM));

	// プレイヤー生成と初期化
    player_ = std::make_unique<Player>();
    player_->SetCamera(camera_);
    player_->Init();
    player_->SetIsInGame(true);

    // マネージャ群（コンボ、スコア）初期化
    comboManager_ = std::make_unique<ComboManager>();
    comboManager_->Init();
    scoreManager_ = std::make_unique<ScoreManager>();
    scoreManager_->Init();

    // 敵マネージャ設定（スコア/コンボ関連付け）
    enemyMgr_.MakeComboAndScoreHandler(comboManager_.get(), scoreManager_.get());
    enemyMgr_.SetBeatAnalyzer(&beatAnalyzer_);
    enemyMgr_.Init(camera_);
    enemyMgr_.SetIsInGame(true);

    player_->SetEnemyManager(&enemyMgr_);

    // レール管理初期化
    railManager_ = std::make_unique<RailManager>();
    railManager_->SetCamera(camera_);
    railManager_->Init();

    railManager_->Reset();

    // 背景（地面）オブジェクト初期化
    ground_ = std::make_unique<Object3d>();
    ground_->Initialize();
    ground_->SetModel("ground.obj");
    groundWT_.Initialize();
    groundWT_.Update();
}

void Stage::Reset()
{
    enemyMgr_.Reset();
    railManager_->Reset();
    comboManager_->Init();
    scoreManager_->Init();
}

void Stage::Update()
{
    isEdit_ = false;

    beatAnalyzer_.Update();
    beatAnalyzer_.Draw();

    // レール更新（カメラ移動・トリガー判定）
    railManager_->Update();

    if (railManager_->RailTrigger())
    {
        // トリガーに到達したとき（敵出現など）
    }

    // プレイヤー更新
    player_->Update();

    // プレイヤー位置を敵側に通知（エイム等のため）
    Vector3 pos = player_->GetWorldPosition();
    enemyMgr_.SetTargetPos(&pos);

    // 敵群の更新
    enemyMgr_.Update();

    // コンボシステム更新
    comboManager_->Update();
}

void Stage::Draw()
{
    // 背景描画
    ground_->Draw(groundWT_);

    // レール・敵・プレイヤー描画
    railManager_->Draw();
    enemyMgr_.Draw();
    player_->Draw();
}

void Stage::DrawUI()
{
    enemyMgr_.DrawUI();
    player_->DrawUI();
}

void Stage::EditUpdate()
{
    isEdit_ = true;
    
    // レールの編集モード更新
    railManager_->UpdateEdit();

    // プレイヤーの更新（編集モード中も表示・動作確認のため）
    player_->Update();
}

nlohmann::json Stage::ToJson() const {
    nlohmann::json j;
    j["rail"] = RailEditor::Instance()->ToJson();
    return j;
}

void Stage::FromJson(const nlohmann::json& j) {
    // レールデータをJSONからデシリアライズ
    RailEditor::Instance()->FromJson(j["rail"]);
    
    // ロード後にレールマネージャを再初期化
    railManager_->Reset();
}
