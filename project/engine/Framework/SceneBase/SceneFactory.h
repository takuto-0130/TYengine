#pragma once
#include "AbstractSceneFactory.h"

// このゲーム用のシーン工場
class SceneFactory :
    public AbstractSceneFactory
{
public:
    /**
     * @brief シーン生成
     * @param sceneName シーン名
     * @return 生成したシーン
     */
    std::unique_ptr<IScene> CreateScene(const std::string& sceneName) override;
};

