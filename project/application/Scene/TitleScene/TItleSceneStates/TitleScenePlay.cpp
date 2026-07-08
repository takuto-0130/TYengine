#include "../TitleScene.h"

using namespace TYEngine;
using namespace Utility;

void TitleSceneStatePlay::Init(TitleScene& owner)
{
	(void)owner;
}

void TitleSceneStatePlay::Update(TitleScene& owner, float deltaTime)
{
	(void)deltaTime;
	(owner.enterSpr_->IsMouseHover()) ? owner.enterSpr_->SetScale(1.2f) : owner.enterSpr_->SetScale(1.0f);

	owner.player_->Update();
	owner.skybox_->Update();
	Vector3 pos = owner.player_->GetWorldPosition();
	owner.enemyMgr_.SetTargetPos(&pos);
	owner.enemyMgr_.Update();
	Vector2 mouse = owner.input_->GetMousePosition();
	owner.reticle_->SetPosition(mouse);

	owner.enterSpr_->SetAlpha(0.5f + (0.5f * owner.oscillator_.GetValue()));
	owner.text_->SetScale(1.0f + (0.07f * owner.oscillator_.GetValue()));

#ifdef _DEBUG
	if (owner.enterSpr_->IsMouseClicked(0))
	{
		owner.stateMachine_.ChangeState(TitleSceneState::FADE_OUT);
	}
#else
	if (owner.input_->TriggerKey(DIK_RETURN) || owner.enterSpr_->IsMouseClicked(0))
	{
		owner.stateMachine_.ChangeState(TitleSceneState::FADE_OUT);
	}
#endif // _DEBUG
}

void TitleSceneStatePlay::Exit(TitleScene& owner)
{
	(void)owner;
}
