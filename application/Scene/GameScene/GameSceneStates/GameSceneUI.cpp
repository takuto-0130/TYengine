#include "../GameScene.h"
#include "../Pause/Pause.h"
#include "../Result/Result.h"
#include "../PlayUI/PlayUI.h"

void GameScene::UIInit()
{
	scoreDraw_ = std::make_unique<Score>();
	scoreDraw_->Initialze();

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
	// フェード中は描画しない
	if (GetCurrentState() != GameSceneState::FADE_OUT && GetCurrentState() != GameSceneState::FADE_IN)
	{
		if (GetCurrentState() != GameSceneState::RESULT)
		{
			playUI_->Draw();
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