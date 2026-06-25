#include "../GameScene.h"
#include "UIManager.h"
#include "../Pause/PauseUI.h"
#include "../Result/ResultUI.h"
#include "../PlayUI/PlayUI.h"
#include "../StartUI/StartUI.h"
#include "../RetryUI/RetryUI.h"
#include "../../../ScoreUI/ScoreUI.h"
#include "SpriteBasis.h"

using namespace TYEngine::Utility;
using namespace TYEngine::Graphics;

void GameScene::UIInit()
{
	auto* uiMgr = UIManager::GetInstance();
	uiMgr->Clear();
	uiMgr->SetJsonManager(gameUIJM_.get());

	// UIの登録
	auto* score = uiMgr->RegisterUI<ScoreUI>("Score");
	uiMgr->RegisterUI<StartUI>("Start");
	auto* play  = uiMgr->RegisterUI<PlayUI>("Play");
	auto* pause = uiMgr->RegisterUI<PauseUI>("Pause");
	uiMgr->RegisterUI<ResultUI>("Result");
	uiMgr->RegisterUI<RetryUI>("Retry");

	// 相互依存関係の設定
	play->SetScoreDraw(score);
	pause->SetConfigJsonManager(configJM_.get());

	// 一括初期化
	uiMgr->InitAll();
}

void GameScene::UIDraw()
{
	SpriteBasis::GetInstance()->BasisDrawSetting();

	// フェード中は描画しない
	if (stateMachine_.GetCurrentState())
	{
		if (stateMachine_.GetCurrentState() != GameSceneState::FADE_OUT)
		{
			auto* uiMgr = UIManager::GetInstance();
			GameSceneState state = stateMachine_.GetCurrentState().value();

			if (state != GameSceneState::RESULT &&
				state != GameSceneState::RETRY)
			{
				uiMgr->DrawUI("Play");
			}
			if (state != GameSceneState::RESULT &&
				state != GameSceneState::READY &&
				state != GameSceneState::RETRY &&
				state != GameSceneState::FADE_IN)
			{
				if (auto* play = uiMgr->GetUI<PlayUI>("Play"))
				{
					play->DrawRT();
				}
			}
			stageManager_->DrawUI();

			if (state == GameSceneState::READY)
			{
				uiMgr->DrawUI("Start");
			}

			if (state == GameSceneState::RESULT)
			{
				uiMgr->DrawUI("Result");
				uiMgr->DrawUI("Score");
				confetti_.Draw();
			}

			if (state == GameSceneState::RETRY)
			{
				uiMgr->DrawUI("Retry");
				uiMgr->DrawUI("Score");
			}

			if (state == GameSceneState::PAUSE)
			{
				uiMgr->DrawUI("Pause");
			}
		}
	}
}