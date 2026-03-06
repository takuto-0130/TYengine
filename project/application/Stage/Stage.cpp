#include "Stage.h"
#include "Timer.h"
#include "BlenderLevelLoader.h"
#include "../Object/Rail/RailEditor.h"
#include "../AppSystem//Audio//GameAudio.h"

using namespace TYEngine::Utility;
using namespace TYEngine::Graphics;

void Stage::Init()
{
    gameAudio_ = GameAudio::GetInstance();

    gameAudio_->InitBeatAnalyzer("418", SoundCategory::BGM);

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
    enemyMgr_.SetBeatAnalyzer(&gameAudio_->GetBeatAnalyzer());
    enemyMgr_.Init(camera_);
    enemyMgr_.SetIsInGame(true);

    player_->SetEnemyManager(&enemyMgr_);

    // レール管理初期化
    railManager_ = std::make_unique<RailManager>();
    railManager_->SetCamera(camera_);
    railManager_->Init();

    railManager_->Reset();

    // 背景（地面）オブジェクト初期化
    BlenderLevelLoader loader;
    loader.DataToObject(loader.Load("stage_object.json"), stageObject_);
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

    // レール更新（カメラ移動・トリガー判定）
    railManager_->Update();

    if (railManager_->RailTrigger())
    {
        // トリガーに到達したとき（敵出現停止）
        enemyMgr_.DisablePopFlag();
    }

    StageObjectBeatScale();
    for (auto&& o : stageObject_)
    {
        o->Update();
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

    // レール・敵・プレイヤー描画
    railManager_->Draw();

    for (auto&& o : stageObject_)
    {
        o->Draw();
    }
    
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

void Stage::StageObjectBeatScale()
{
    static float timer = 0.0f;
    for (auto& o : stageObject_)
    {
        if (o->GetModelName() == "conifer.obj")
        {
            if (gameAudio_->GetBeatAnalyzer().GetBeat())
            {
                timer = 0.0f;
                o->SetScale(1.05f);
            }
            else
            {
                if (timer < 1.0f)
                {
                    timer += Timer::GetInstance()->GetDeltaTime();
                }
                o->SetScale(Lerp(1.05f, 1.0f, timer));
            }
        }
    }
}
