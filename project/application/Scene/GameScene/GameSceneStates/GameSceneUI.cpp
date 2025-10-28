#include "../GameScene.h"
#include "../Pause/Pause.h"
#include "../Result/Result.h"
#include "../PlayUI/PlayUI.h"
#include "../StartUI/StartUI.h"
#include "../../../ScoreUI/ScoreUI.h"
#include "SpriteBasis.h"

void GameScene::UIInit()
{
	scoreDraw_ = std::make_unique<ScoreUI>();
	scoreDraw_->Initialze();

	startDraw_ = std::make_unique<StartUI>();
	startDraw_->Initialze();

	playUI_ = std::make_unique<PlayUI>();
	playUI_->SetScoreDraw(scoreDraw_.get());
	playUI_->Init();

	pauseMenu_ = std::make_unique<PauseClass>();
	pauseMenu_->Initialze();

	resultMenu_ = std::make_unique<ResultClass>();
	resultMenu_->Initialze();
}

void GameScene::UIDraw()
{
	SpriteBasis::GetInstance()->BasisDrawSetting();

	// フェード中は描画しない
	if (/*GetCurrentState() != GameSceneState::FADE_OUT && GetCurrentState() != GameSceneState::FADE_IN*/true)
	{
		if (GetCurrentState() != GameSceneState::RESULT &&
			GetCurrentState() != GameSceneState::RETRY)
		{
			playUI_->Draw();
		}
		if (GetCurrentState() != GameSceneState::RESULT &&
			GetCurrentState() != GameSceneState::READY &&
			GetCurrentState() != GameSceneState::RETRY &&
			GetCurrentState() != GameSceneState::FADE_IN)
		{
			playUI_->DrawRT();
		}

		if (GetCurrentState() == GameSceneState::READY)
		{
			startDraw_->Draw();
		}

		if (GetCurrentState() == GameSceneState::RESULT)
		{
			resultMenu_->Draw();
			scoreDraw_->Draw();
		}

		if (GetCurrentState() == GameSceneState::PAUSE)
		{
			pauseMenu_->Draw();
		}
	}
}