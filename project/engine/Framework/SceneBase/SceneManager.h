#pragma once
#include "IScene.h"
#include "AbstractSceneFactory.h"
#include "SingletonObject.h"

namespace TYEngine {
namespace Framework {

using namespace Utility;

/// <summary>
/// ゲームシーンの管理を行うクラス。  
/// 現在のシーンの更新・描画・切り替えを制御し、  
/// シーン遷移時のメモリ管理や初期化を自動で行うシングルトンクラス。
/// </summary>
class SceneManager :
    public Utility::SingletonObject<SceneManager>
{
    friend class Utility::SingletonObject<SceneManager>;
    friend struct std::default_delete<SceneManager>;

private:
    // 外部からの new/delete を禁止
    SceneManager() = default;
    ~SceneManager();

public:
    /// <summary>
    /// 次に切り替えるシーンを予約する。  
    /// 現在のシーン更新後に切り替えが実行される。
    /// </summary>
    /// <param name="sceneName">遷移先シーン名。</param>
    void ChangeScene(const std::string& sceneName);

    /// <summary>
    /// 現在のシーンを更新する。  
    /// シーン遷移の予約があれば自動的に切り替えを行う。
    /// </summary>
    void Update();

    /// <summary>
    /// 現在のシーンを描画する。  
    /// シーンが存在しない場合は何もしない。
    /// </summary>
    void Draw();

    /// <summary>
    /// 現在のシーンに属する UI の描画を行う。  
    /// ImGui などのデバッグ用 UI 表示を担当。
    /// </summary>
    void UIDraw();

    /// <summary>
    /// シーン生成ファクトリを登録する。  
    /// 登録されたファクトリを通してシーンを生成・破棄する。
    /// </summary>
    /// <param name="sceneFactory">シーン生成ファクトリのインスタンス。</param>
    void SetSceneFactory(AbstractSceneFactory* sceneFactory) { sceneFactory_ = sceneFactory; }

private:
    /// <summary>現在アクティブなシーン。</summary>
    std::unique_ptr<IScene> scene_ = nullptr;

    /// <summary>次に切り替え予定のシーン。</summary>
    std::unique_ptr<IScene> nextScene_ = nullptr;

    /// <summary>シーンを生成するためのファクトリ。</summary>
    AbstractSceneFactory* sceneFactory_ = nullptr;
};

} // namespace Framework
} // namespace TYEngine
