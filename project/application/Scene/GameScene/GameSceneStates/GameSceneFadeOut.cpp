#include "../GameScene.h"
#include "../../Transition/Fade/FadeTransition.h"
#include "../../Transition/TransitionManager.h"
#include "SceneManager.h"

void GameScene::InitFadeOut()
{
	Audio::GetInstance()->StopBGM(BGMHandle_);
	auto transition = std::make_unique<FadeTransition>(FadeTransition::Type::FADE_OUT, 1.0f);
	transition->SetOnFinishCallback([this]() {
		sceneManager_->ChangeScene("TITLE");
		TransitionManager::GetInstance()->Enqueue(std::make_unique<FadeTransition>(FadeTransition::Type::FADE_IN, 1.0f));
		});
	TransitionManager::GetInstance()->Start(std::move(transition));
}
void GameScene::UpdateFadeOut()
{
}
void GameScene::ExitFadeOut()
{
}