#include "../TitleScene.h"

using namespace TYEngine;
using namespace Utility;

void TitleScene::InitPlay()
{}

void TitleScene::UpdatePlay()
{
	beatAnalyzer_.Update();
	audioAnalyzer_.Update();
	(enterSpr_->IsMouseHover()) ? enterSpr_->SetScale(1.2f) : enterSpr_->SetScale(1.0f);

	player_->Update();
	skybox_->Update();
	Vector3 pos = player_->GetWorldPosition();
	enemyMgr_.SetTargetPos(&pos);
	enemyMgr_.Update();
	Vector2 mouse = input_->GetMousePosition();
	reticle_->SetPosition(mouse);

	enterSpr_->SetAlpha(0.5f + (0.5f * oscillator_.GetValue()));
	text_->SetScale(1.0f + (0.07f * oscillator_.GetValue()));

#ifdef _DEBUG
	if (enterSpr_->IsMouseClicked(0))
	{
		stateMachine_.ChangeState(TitleSceneState::FADE_OUT);
	}
#else
	if (input_->TriggerKey(DIK_RETURN) || enterSpr_->IsMouseClicked(0))
	{
		stateMachine_.ChangeState(TitleSceneState::FADE_OUT);
	}
#endif // _DEBUG
}

void TitleScene::ExitPlay()
{}
