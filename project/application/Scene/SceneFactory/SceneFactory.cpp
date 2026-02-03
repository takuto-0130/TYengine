#include "SceneFactory.h"
#include "Scene/TitleScene/TitleScene.h"
#include "Scene/GameScene/GameScene.h"


std::unique_ptr<IScene> SceneFactory::CreateScene(const std::string& sceneName)
{
    // 文字列に応じたシーンクラスの生成
    if (sceneName == "TITLE") return std::make_unique<TitleScene>();
    if (sceneName == "GAME")  return std::make_unique<GameScene>();
    return nullptr;
}
