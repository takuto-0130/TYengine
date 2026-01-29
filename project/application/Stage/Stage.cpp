#include "Stage.h"
#include "../Object/Rail/RailEditor.h"

void Stage::Init()
{
    player_ = std::make_unique<Player>();
    player_->SetCamera(camera_);
    player_->Init();

    comboManager_ = std::make_unique<ComboManager>();
    comboManager_->Init();
    scoreManager_ = std::make_unique<ScoreManager>();
    scoreManager_->Init();


    enemyMgr_.MakeComboAndScoreHandler(comboManager_.get(), scoreManager_.get());
    enemyMgr_.Init(camera_);

    railManager_ = std::make_unique<RailManager>();
    railManager_->SetCamera(camera_);
    railManager_->Init();

    Reset();

    ground_ = std::make_unique<Object3d>();
    ground_->Initialize();
    ground_->SetModel("ground.obj");
    groundWT_.Initialize();
    groundWT_.Update();
}

void Stage::Reset()
{
    railManager_->Reset();
}

void Stage::Update()
{
    isEdit_ = false;

    enemyMgr_.SetCamera(camera_);

    railManager_->Update();

    if (railManager_->RailTrigger())
    {
        // トリガーに到達したとき
    }

    player_->Update();

    Vector3 pos = player_->GetWorldPosition();

    enemyMgr_.SetTargetPos(&pos);

    enemyMgr_.Update();

    comboManager_->Update();
}

void Stage::Draw()
{
    ground_->Draw(groundWT_);

    railManager_->Draw();
    enemyMgr_.Draw();

    player_->Draw();
}

void Stage::EditUpdate()
{
    isEdit_ = true;
    railManager_->UpdateEdit();

    player_->Update();
}

nlohmann::json Stage::ToJson() const {
    nlohmann::json j;
    j["rail"] = RailEditor::Instance()->ToJson();
    return j;
}

void Stage::FromJson(const nlohmann::json& j) {
    RailEditor::Instance()->FromJson(j["rail"]);
    railManager_->Reset();
}
