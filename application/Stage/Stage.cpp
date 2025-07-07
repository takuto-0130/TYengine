#include "Stage.h"
#include "../Object/Rail/RailEditor.h"

void Stage::Init()
{
    player_ = std::make_unique<Player>();
    player_->SetCamera(camera_);
    player_->Init();

    enemyManager_ = std::make_unique<EnemyManager>();
    enemyManager_->Init();

    railManager_ = std::make_unique<RailManager>();
    railManager_->SetCamera(camera_);
    railManager_->Init();

    //skydome_ = std::make_unique<Skydome>();
    //skydome_->Initialize();

    // 1フレームだけカメラを動かす
    railManager_->RailCameraMove();
}

void Stage::Update()
{
    isEdit_ = false;
    enemyManager_->Update();

    railManager_->Update();

    if (railManager_->RailTrigger()) enemyManager_->TriggerNextEnemyGroup();

    player_->Update();

    //skydome_->Update();
}

void Stage::Draw()
{
    //skydome_->Draw();

    railManager_->Draw();

    if (isEdit_) {
#ifdef _DEBUG
        enemyManager_->DrawEditorEnemies();
#endif // _DEBUG
    }
    else
    {
        enemyManager_->Draw();
    }

    player_->Draw();
}

void Stage::EditUpdate()
{
    isEdit_ = true;
    railManager_->UpdateEdit();
    enemyManager_->DrawEditorUI();
    enemyManager_->UpdateEditorEnemies();

    player_->Update();
    //skydome_->Update();
}

nlohmann::json Stage::ToJson() const {
    nlohmann::json j;
    j["enemy"] = enemyManager_->GetEditor()->ToJson();
    j["rail"] = RailEditor::Instance()->ToJson();
    return j;
}

void Stage::FromJson(const nlohmann::json& j) {
    enemyManager_->GetEditor()->FromJson(j["enemy"]);
    RailEditor::Instance()->FromJson(j["rail"]);
}
