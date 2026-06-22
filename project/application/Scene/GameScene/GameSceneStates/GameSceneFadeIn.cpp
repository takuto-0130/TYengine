#include "../GameScene.h"
#include "../../Transition/Fade/FadeTransition.h"
#include "../../Transition/TransitionManager.h"
#include "../../../Object/Rail/RailManager.h"

void GameSceneStateFadeIn::Init(GameScene& owner)
{
	(void)owner;
}

void GameSceneStateFadeIn::Update(GameScene& owner, float deltaTime)
{
	(void)deltaTime;
	if (owner.readyCount_ < 2)
	{
		++owner.readyCount_;
		// UIやプレイヤーの初期更新を数フレーム回して安定させる
		owner.ComboUIUpdate();
		owner.PlayUIUpdate();
		owner.stageManager_->GetPlayer()->Update();
	}
	if(owner.readyCount_ == 2)
	{
		++owner.readyCount_;
		// オープニングカメラ演出の開始
		owner.StartCamera();
	}

	if (!TransitionManager::GetInstance()->IsBusy())
	{
		owner.stateMachine_.ChangeState(GameSceneState::PLAY);
	}
}

void GameSceneStateFadeIn::Exit(GameScene& owner)
{
	(void)owner;
}