#include "Stage.h"
#include "../Object/Rail/RailEditor.h"

void Stage::Init()
{
    player_ = std::make_unique<Player>();
    player_->Init();

    enemyManager_ = std::make_unique<EnemyManager>();
    enemyManager_->Init();

    railManager_ = std::make_unique<RailManager>();
    railManager_->Init();

    skydome_ = std::make_unique<Skydome>();
    skydome_->Initialize();
}

void Stage::Update()
{

}

void Stage::Draw()
{

}

void Stage::EditUpdate()
{

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
