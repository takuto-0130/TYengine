#include "Stage.h"
#include "Timer.h"
#include "BlenderLevelLoader.h"
#include "../AppSystem//Audio//GameAudio.h"
#include "../AppSystem/RailGenerator/AppRailGenerator.h"

using namespace TYEngine::Utility;
using namespace TYEngine::Graphics;

Stage::~Stage()
{
}

void Stage::Init()
{
    gameAudio_ = GameAudio::GetInstance();

    gameAudio_->InitBeatAnalyzer("418", SoundCategory::BGM);

	// プレイヤー生成と初期化
    player_ = std::make_unique<Player>();
    player_->SetCamera(camera_);
    player_->Init();
    player_->SetIsInGame(true);
    player_->SetBeatAnalyzer(&gameAudio_->GetBeatAnalyzer());

    // マネージャ群（コンボ、スコア）初期化
    comboManager_ = std::make_unique<HitStreakManager>();
    comboManager_->Init();
    scoreManager_ = std::make_unique<ScoreManager>();
    scoreManager_->Init();

    // 敵マネージャ設定（スコア/コンボ関連付け）
    enemyMgr_.MakeComboAndScoreHandler(comboManager_.get(), scoreManager_.get());
    enemyMgr_.SetBeatAnalyzer(&gameAudio_->GetBeatAnalyzer());
    enemyMgr_.Init(camera_);
    enemyMgr_.SetIsInGame(true);

    player_->SetEnemyManager(&enemyMgr_);

    // レール管理/背景（地面）オブジェクト初期化
    railManager_ = std::make_unique<RailManager>();
    railManager_->SetCamera(camera_);
    railManager_->Init();

    // EnemyManagerに地形の高さを計算させるために渡す
    enemyMgr_.SetRailManager(railManager_.get());

    GenerateStageFromAudio("418", railManager_.get());
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

    DebugUI();

    // レール更新（カメラ移動・トリガー判定）
    railManager_->Update();

    if (railManager_->RailTrigger())
    {
        // トリガーに到達したとき（敵出現停止）
        enemyMgr_.DisablePopFlag();
    }
    Vector3 pos = player_->GetWorldPosition();
    enemyMgr_.SetTargetPos(&pos);
    enemyMgr_.Update();

    StageObjectBeatScale();
    for (auto&& o : stageObject_)
    {
        o->Update();
    }

    // プレイヤー更新
    player_->Update();

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
    //j["rail"] = RailEditor::Instance()->ToJson();
    return j;
}

void Stage::FromJson(const nlohmann::json& j) {
    // レールデータをJSONからデシリアライズ
    //RailEditor::Instance()->FromJson(j["rail"]);
    (void)j;
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

void Stage::DebugUI()
{
    ImGui::Begin("Sound Selector");

    // ImGui::Combo に渡すための C言語風文字列配列 (const char*) を作成
    std::vector<const char*> comboItems;
    for (const auto& song : songList_)
    {
        comboItems.push_back(song.c_str());
    }

    // プルダウンメニューの表示
    // ユーザーが別の曲を選択して値が変更された場合、ifの中に入る
    if (ImGui::Combo("Select Song", &currentSongIndex_, comboItems.data(), static_cast<int>(comboItems.size())))
    {
        gameAudio_->Pause(BGMHandle_);
        BGMHandle_ = gameAudio_->Play(songList_[currentSongIndex_], true, SoundCategory::BGM);
        railManager_->Reset();
        // 選択された新しい曲でレールを再生成し、レールマネージャーに流し込む
        GenerateStageFromAudio(songList_[currentSongIndex_], railManager_.get());
    }

    ImGui::End();
}
