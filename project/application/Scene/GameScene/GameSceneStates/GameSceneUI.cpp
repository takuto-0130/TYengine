#include "../GameScene.h"
#include "../Pause/Pause.h"
#include "../Result/Result.h"
#include "../PlayUI/PlayUI.h"
#include "../StartUI/StartUI.h"
#include "../RetryUI/RetryUI.h"
#include "../../../ScoreUI/ScoreUI.h"
#include "SpriteBasis.h"

using namespace TYEngine::Utility;
using namespace TYEngine::Graphics;

void GameScene::UIInit()
{
	scoreDraw_ = std::make_unique<ScoreUI>();
	scoreDraw_->SetJsonManager(&gameUIJM_);
	scoreDraw_->Init();

	startDraw_ = std::make_unique<StartUI>();
	startDraw_->SetJsonManager(&gameUIJM_);
	startDraw_->Init();

	playUI_ = std::make_unique<PlayUI>();
	playUI_->SetJsonManager(&gameUIJM_);
	playUI_->SetScoreDraw(scoreDraw_.get());
	playUI_->Init();

	pauseMenu_ = std::make_unique<PauseClass>();
	pauseMenu_->SetJsonManager(&gameUIJM_);
	pauseMenu_->SetConfigJsonManager(&configJM_);
	pauseMenu_->Init();

	resultMenu_ = std::make_unique<ResultClass>();
	resultMenu_->SetJsonManager(&gameUIJM_);
	resultMenu_->Init();

	retryDraw_ = std::make_unique<RetryUI>();
	retryDraw_->SetJsonManager(&gameUIJM_);
	retryDraw_->Init();
}

void GameScene::UIDraw()
{
	SpriteBasis::GetInstance()->BasisDrawSetting();

	// フェード中は描画しない
	if (GetCurrentState() != GameSceneState::FADE_OUT)
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
			confetti_.Draw();
		}

		if (GetCurrentState() == GameSceneState::RETRY)
		{
			retryDraw_->Draw();
			scoreDraw_->Draw();
		}

		if (GetCurrentState() == GameSceneState::PAUSE)
		{
			pauseMenu_->Draw();
		}
	}
}