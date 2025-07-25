#include "SceneFactory.h"
#include "Scene/TitleScene/TitleScene.h"
#include "Scene/GameScene/GameScene.h"


IScene* SceneFactory::CreateScene(const std::string& sceneName)
{
	IScene* newScene = nullptr;

	if (sceneName == "TITLE") {
		newScene = new TitleScene();
	}
	else if (sceneName == "GAME") {
		newScene = new GameScene();
	}

	return newScene;
}
