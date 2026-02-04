#include "SceneManager.h"
#include "EngineConfig.h"
#include "Timer.h"
#include "../../../application/Scene/Transition/TransitionManager.h"

SceneManager::~SceneManager()
{
}

void SceneManager::ChangeScene(const std::string& sceneName)
{
	assert(sceneFactory_);
	// 次のシーンを生成して予約
	nextScene_ = sceneFactory_->CreateScene(sceneName);
}

void SceneManager::Update()
{
	// シーン切り替え機構
	// 次シーンの予約があるなら
	if (nextScene_) {
		// シーン切り替え
		scene_ = std::move(nextScene_);

		// シーンマネージャーをセット
		scene_->SetSceneManager(this);

		// シーンの初期化
		scene_->Init();
	}

	// 実行中シーンを更新する
	scene_->Update();

	// 画面遷移は固定フレームで進行
	TransitionManager::GetInstance()->Update(1.0f / EngineConfig::fps_);
}

void SceneManager::Draw()
{
	if (scene_) scene_->Draw();
}

void SceneManager::UIDraw()
{
	if (scene_) scene_->UIDraw();
	TransitionManager::GetInstance()->Draw();
}
