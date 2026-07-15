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
	float hoverScale = owner.titleJM.Get<float>("config.animation.hoverScale", 1.2f);
	(owner.enterSpr_->IsMouseHover()) ? owner.enterSpr_->SetScale(hoverScale) : owner.enterSpr_->SetScale(1.0f);

	owner.player_->Update();
	owner.skybox_->Update();
	Vector3 pos = owner.player_->GetWorldPosition();
	owner.enemyMgr_.SetTargetPos(&pos);
	owner.enemyMgr_.Update();
	Vector2 mouse = owner.input_->GetMousePosition();
	owner.reticle_->SetPosition(mouse);

	float pulseAlphaBase = owner.titleJM.Get<float>("config.animation.pulseAlphaBase", 0.5f);
	float pulseAlphaAmp = owner.titleJM.Get<float>("config.animation.pulseAlphaAmp", 0.5f);
	float logoScaleBase = owner.titleJM.Get<float>("config.animation.logoScaleBase", 1.0f);
	float logoScaleAmp = owner.titleJM.Get<float>("config.animation.logoScaleAmp", 0.07f);

	owner.enterSpr_->SetAlpha(pulseAlphaBase + (pulseAlphaAmp * owner.oscillator_.GetValue()));
	owner.text_->SetScale(logoScaleBase + (logoScaleAmp * owner.oscillator_.GetValue()));

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
