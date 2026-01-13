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

    enemyManager_ = std::make_unique<EnemyManager>();
    enemyManager_->MakeComboAndScoreHandler(comboManager_.get(), scoreManager_.get());
    enemyManager_->Init();

    enemyMgr_.Init(camera_);

    railManager_ = std::make_unique<RailManager>();
    railManager_->SetCamera(camera_);
    railManager_->Init();

    Reset();

    ground_ = std::make_unique<Object3d>();
    ground_->Initialize();
    ground_->SetModel("ground.obj");
    groundWT_.Initialize();
    groundWT_.TransferMatrix();
}

void Stage::Reset()
{
    //enemyManager_->Reset();
    railManager_->Reset();
}

void Stage::Update()
{
    isEdit_ = false;
    //enemyManager_->SetCamera(camera_);
    //enemyManager_->Update();

    enemyMgr_.SetCamera(camera_);

    railManager_->Update();

    if (railManager_->RailTrigger())
    {

    }

    player_->Update();

    Vector3 pos = player_->GetWorldPosition();
    //enemyManager_->SetTargetPos(&pos);

    enemyMgr_.SetTargetPos(&pos);

    enemyMgr_.Update();

    comboManager_->Update();
}

void Stage::Draw()
{
    ground_->Draw(groundWT_);

    railManager_->Draw();
    enemyMgr_.Draw();
    if (isEdit_) {
#ifdef _DEBUG
        //enemyManager_->DrawEditorEnemies();
#endif // _DEBUG
    }
    else
    {
        //enemyManager_->Draw();
    }

    player_->Draw();
}

void Stage::EditUpdate()
{
    isEdit_ = true;
    railManager_->UpdateEdit();
    //enemyManager_->DrawEditorUI();
    //enemyManager_->UpdateEditorEnemies();

    player_->Update();
}

nlohmann::json Stage::ToJson() const {
    nlohmann::json j;
    j["enemy"] = enemyManager_->GetEditor()->ToJson();
    j["rail"] = RailEditor::Instance()->ToJson();
    return j;
}

void Stage::FromJson(const nlohmann::json& j) {
    enemyManager_->GetEditor()->FromJson(j["enemy"]);
    enemyManager_->Reset();
    RailEditor::Instance()->FromJson(j["rail"]);
    railManager_->Reset();
}
