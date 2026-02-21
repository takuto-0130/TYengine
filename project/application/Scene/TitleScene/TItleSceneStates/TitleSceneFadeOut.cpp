#include "../TitleScene.h"
#include "../../Transition/Fade2/BlockFadeTransition.h"
#include "../../Transition/TransitionManager.h"
#include "SceneManager.h"
#include "../../../AppSystem/Audio/GameAudio.h"


using namespace TYEngine;
using namespace AudioSystem;

void TitleScene::InitFadeOut()
{
#ifdef _DEBUG
	sceneManager_->ChangeScene("GAME");
	gameAudio_->Stop(bgmHandle_);
#else
	// 決定音再生
	gameAudio_->Play("enter", false, SoundCategory::UI);

	// ブロックフェード演出を開始してゲームシーンへ遷移
	BlockFadeConfig cfg;
	auto transition = std::make_unique<BlockFadeTransition>(BlockFadeTransition::Type::FADE_IN, cfg);
	transition->SetOnFinishCallback([this]()
		{
			BlockFadeConfig cfg1;
			sceneManager_->ChangeScene("GAME");
			TransitionManager::GetInstance()->Enqueue(std::make_unique<BlockFadeTransition>(BlockFadeTransition::Type::FADE_OUT, cfg1));
			gameAudio_->Stop(bgmHandle_);
		});
	TransitionManager::GetInstance()->Start(std::move(transition));
#endif // _DEBUG
}

void TitleScene::UpdateFadeOut()
{}

void TitleScene::ExitFadeOut()
{}