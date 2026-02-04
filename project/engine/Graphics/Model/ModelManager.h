#pragma once
#include <map>
#include <memory>
#include <string>
#include "Model.h"
#include <mutex>
#include <assimp/Importer.hpp>
#include <assimp/scene.h>
#include <assimp/postprocess.h>
#include "TextureManager.h"
#include "SingletonObject.h"

namespace TYEngine {
namespace Graphics {

using namespace Core; // For DirectXBasis

/// <summary>
/// 3D モデルの読み込み・管理を一元化するシングルトンクラス。  
/// ファイルパスをキーに Model をキャッシュし、重複読み込みを防ぐ。
/// </summary>
class ModelManager :
    public SingletonObject<ModelManager>
{
    friend class SingletonObject<ModelManager>;
    friend struct std::default_delete<ModelManager>;

private:
    // 外部からの new/delete を禁止
    ModelManager() = default;
    ~ModelManager() = default;

public:

    /// <summary>
    /// 初期化処理。モデル読み込みに必要な DirectX 基盤と SRV 管理を登録する。
    /// </summary>
    /// <param name="dxManager">DirectX 基盤（デバイス・コマンド関連）。</param>
    /// <param name="srvManager">SRV 管理クラス。</param>
    void Initialize(DirectXBasis* dxManager, SrvManager* srvManager);
    /// <summary>
    /// モデルファイルを読み込み、管理下に追加する。  
    /// 同じファイルは再読み込みせずキャッシュを使用する。
    /// </summary>
    /// <param name="directoryPath">モデルのディレクトリパス。</param>
    /// <param name="filePath">モデルファイル名（または相対パス）。</param>
    void LoadModel(const std::string& directoryPath, const std::string& filePath);

    /// <summary>
    /// すでに読み込まれているモデルを検索して取得する。
    /// </summary>
    /// <param name="filePath">検索するモデルのファイルパス（キー）。</param>
    /// <returns>該当する Model。存在しない場合は nullptr。</returns>
    Model* FindModel(const std::string& filePath);
    /// <summary>
    /// 使用中の ModelLoader を取得する。
    /// </summary>
    /// <returns>ModelLoader インスタンス。</returns>
    ModelLoader* GetModelLoader() { return modelLoader_.get(); }

private:
    /// <summary>
    /// 読み込まれたモデルの辞書。キーはファイルパス。
    /// </summary>
    std::map<std::string, std::unique_ptr<Model>> models_;

    /// <summary>
    /// 実際の読み込み処理を担うローダ。
    /// </summary>
    std::unique_ptr<ModelLoader> modelLoader_ = nullptr;
};

} // namespace Graphics
} // namespace TYEngine
