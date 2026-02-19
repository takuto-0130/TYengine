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
	scoreDraw_->SetJsonManager(gameUIJM_.get());
	scoreDraw_->Init();

	startDraw_ = std::make_unique<StartUI>();
	startDraw_->SetJsonManager(gameUIJM_.get());
	startDraw_->Init();

	playUI_ = std::make_unique<PlayUI>();
	playUI_->SetJsonManager(gameUIJM_.get());
	playUI_->SetScoreDraw(scoreDraw_.get());
	playUI_->Init();

	pauseMenu_ = std::make_unique<PauseClass>();
	pauseMenu_->SetJsonManager(gameUIJM_.get());
	pauseMenu_->SetConfigJsonManager(configJM_.get());
	pauseMenu_->Init();

	resultMenu_ = std::make_unique<ResultClass>();
	resultMenu_->SetJsonManager(gameUIJM_.get());
	resultMenu_->Init();

	retryDraw_ = std::make_unique<RetryUI>();
	retryDraw_->SetJsonManager(gameUIJM_.get());
	retryDraw_->Init();
}

void GameScene::UIDraw()
{
	SpriteBasis::GetInstance()->BasisDrawSetting();

	// フェード中は描画しない
	if (stateMachine_.GetCurrentState() != GameSceneState::FADE_OUT)
	{
		if (stateMachine_.GetCurrentState() != GameSceneState::RESULT &&
			stateMachine_.GetCurrentState() != GameSceneState::RETRY)
		{
			playUI_->Draw();
		}
		if (stateMachine_.GetCurrentState() != GameSceneState::RESULT &&
			stateMachine_.GetCurrentState() != GameSceneState::READY &&
			stateMachine_.GetCurrentState() != GameSceneState::RETRY &&
			stateMachine_.GetCurrentState() != GameSceneState::FADE_IN)
		{
			playUI_->DrawRT();
		}
		stageManager_->DrawUI();

		if (stateMachine_.GetCurrentState() == GameSceneState::READY)
		{
			startDraw_->Draw();
		}

		if (stateMachine_.GetCurrentState() == GameSceneState::RESULT)
		{
			resultMenu_->Draw();
			scoreDraw_->Draw();
			confetti_.Draw();
		}

		if (stateMachine_.GetCurrentState() == GameSceneState::RETRY)
		{
			retryDraw_->Draw();
			scoreDraw_->Draw();
		}

		if (stateMachine_.GetCurrentState() == GameSceneState::PAUSE)
		{
			pauseMenu_->Draw();
		}
	}
}