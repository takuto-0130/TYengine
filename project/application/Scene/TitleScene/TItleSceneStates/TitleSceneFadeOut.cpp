#include "../TitleScene.h"
#include "../../Transition/Fade2/BlockFadeTransition.h"
#include "../../Transition/TransitionManager.h"
#include "SceneManager.h"
#include "../../../AppSystem/Audio/GameAudio.h"

using namespace TYEngine;
using namespace AudioSystem;

void TitleSceneStateFadeOut::Init(TitleScene& owner)
{
#ifdef _DEBUG
	owner.sceneManager_->ChangeScene("GAME");
	owner.gameAudio_->Stop(owner.bgmHandle_);
#else
	// 決定音再生
	owner.gameAudio_->Play("enter", false, SoundCategory::UI);

	// ブロックフェード演出を開始してゲームシーンへ遷移
	BlockFadeConfig cfg;
	auto transition = std::make_unique<BlockFadeTransition>(BlockFadeTransition::Type::FADE_IN, cfg);
	transition->SetOnFinishCallback([owner_ptr = &owner]()
		{
			BlockFadeConfig cfg1;
			owner_ptr->sceneManager_->ChangeScene("GAME");
			TransitionManager::GetInstance()->Enqueue(std::make_unique<BlockFadeTransition>(BlockFadeTransition::Type::FADE_OUT, cfg1));
			owner_ptr->gameAudio_->Stop(owner_ptr->bgmHandle_);
		});
	TransitionManager::GetInstance()->Start(std::move(transition));
#endif // _DEBUG
}

void TitleSceneStateFadeOut::Update(TitleScene& owner, float deltaTime)
{
	(void)owner;
	(void)deltaTime;
}

void TitleSceneStateFadeOut::Exit(TitleScene& owner)
{
	(void)owner;
}