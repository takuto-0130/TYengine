#include "../GameScene.h"
#include "../../Transition/Fade/FadeTransition.h"
#include "../../Transition/TransitionManager.h"
#include "SceneManager.h"

using namespace TYEngine;

void GameSceneStateFadeOut::Init(GameScene& owner)
{
	AudioSystem::Audio::GetInstance()->Stop(owner.BGMHandle_);
	auto transition = std::make_unique<FadeTransition>(FadeTransition::Type::FADE_OUT, 1.0f);
	transition->SetOnFinishCallback([owner_ptr = &owner]() {
		owner_ptr->sceneManager_->ChangeScene("TITLE");
		TransitionManager::GetInstance()->Enqueue(std::make_unique<FadeTransition>(FadeTransition::Type::FADE_IN, 1.0f));
		});
	TransitionManager::GetInstance()->Start(std::move(transition));
}

void GameSceneStateFadeOut::Update(GameScene& owner, float deltaTime)
{
	(void)owner;
	(void)deltaTime;
}

void GameSceneStateFadeOut::Exit(GameScene& owner)
{
	(void)owner;
}