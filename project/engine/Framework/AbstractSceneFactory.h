#pragma once

#include "IScene.h"
#include <string>

/// <summary>
/// シーン生成ファクトリの抽象基底インターフェース。
/// シーン名をキーにして具体的な IScene インスタンスを生成する。
/// </summary>
class AbstractSceneFactory
{
public:
	/// <summary>仮想デストラクタ。</summary>
	virtual ~AbstractSceneFactory() = default;

	/// <summary>
	/// 指定されたシーン名に基づいてシーンインスタンスを生成する。
	/// </summary>
	/// <param name="sceneName">生成するシーンの名前。</param>
	/// <returns>生成されたシーン（ユニークポインタ）。</returns>
	virtual std::unique_ptr<IScene> CreateScene(const std::string& sceneName) = 0;
};