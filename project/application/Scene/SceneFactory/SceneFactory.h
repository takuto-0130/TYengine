#pragma once
#include "AbstractSceneFactory.h"

// このゲーム用のシーン工場
/// <summary>
/// ゲーム固有のシーン生成ファクトリ。
/// 文字列（シーン名）に応じて具体的なシーンインスタンスを生成する。
/// </summary>
class SceneFactory :
    public TYEngine::Framework::AbstractSceneFactory
{
public:
    /// <summary>
    /// シーン生成を行う。
    /// </summary>
    /// <param name="sceneName">生成するシーンの名前。</param>
    /// <returns>生成されたシーンのユニークポインタ。</returns>
    std::unique_ptr<TYEngine::Framework::IScene> CreateScene(const std::string& sceneName) override;
};

