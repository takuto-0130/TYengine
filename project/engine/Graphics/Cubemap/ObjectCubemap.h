#pragma once
#include "Model.h"
#include "ModelManager.h"
#include "Camera.h"
#include "WorldTransform.h"
#include "CubemapBasis.h"

const int indexNum = 36;

namespace TYEngine {
namespace Graphics {

using namespace Utility::SceneObjects; // For WorldTransform
using namespace Camera; // For Camera

/// <summary>
/// キューブマップ（スカイボックス）を描画するオブジェクト。  
/// カメラ、マテリアル、頂点・インデックス、行列などの GPU リソースを保持し、更新・描画を行う。
/// </summary>
class ObjectCubemap
{
public:
    /// <summary>デフォルトコンストラクタ。</summary>
    ObjectCubemap() = default;

    /// <summary>デストラクタ。</summary>
    ~ObjectCubemap() = default;

    /// <summary>
    /// 初期化処理。  
    /// 指定されたキューブマップテクスチャを読み込み、各種リソースを生成する。
    /// </summary>
    /// <param name="textureFilePath">使用するキューブマップのテクスチャファイルパス。</param>
    void Initialize(const std::string& textureFilePath);

    /// <summary>
    /// 毎フレームの更新処理。  
    /// カメラ行列やワールド行列の更新、行列バッファへの反映を行う。
    /// </summary>
    void Update();

    /// <summary>
    /// 描画処理。  
    /// CubemapBasis のパイプライン設定を利用して、スカイボックスを描画する。
    /// </summary>
    void Draw();

    /// <summary>
    /// 使用するカメラを設定する。
    /// </summary>
    /// <param name="camera">カメラインスタンス。</param>
    void SetCamera(Camera::Camera* camera) { camera_ = camera; }

private:
    /// <summary>
    /// カメラ用の定数バッファを生成・マップする。
    /// </summary>
    void CreateCameraResource();

    /// <summary>
    /// 頂点・インデックスデータを作成し、バッファを生成する。
    /// </summary>
    void CreateVertexData();

    /// <summary>
    /// マテリアル用の定数バッファを生成・マップする。
    /// </summary>
    void CreateMaterialResource();

    /// <summary>
    /// 座標変換行列用の定数バッファを生成・マップする。
    /// </summary>
    void CreateTransformationMatrixResource();

private:
    /// <summary>使用するカメラ。</summary>
    Camera::Camera* camera_ = nullptr;

    // ========================
    //        Camera CB
    // ========================
    /// <summary>
    /// カメラ定数バッファ。  
    /// worldPosition：カメラのワールド座標（反射・環境マップ処理で利用可能）。
    /// </summary>
    struct CameraForGPU
    {
        Vector3 worldPosition;
    };
    Microsoft::WRL::ComPtr<ID3D12Resource> cameraResource_; ///< カメラ CB リソース。
    CameraForGPU* cameraData_ = nullptr;                    ///< カメラ CB マップ先。

    // ========================
    //      Material CB
    // ========================
    /// <summary>
    /// キューブマップ用の簡易マテリアル。  
    /// スカイボックスのカラー調整などに使用可能。
    /// </summary>
    struct CubeMaterial
    {
        Vector4 color; ///< 乗算カラー（RGBA）。
    };
    Microsoft::WRL::ComPtr<ID3D12Resource> materialResource_; ///< マテリアル CB リソース。
    CubeMaterial* materialData_ = nullptr;                    ///< マテリアル CB マップ先。

    // ========================
    //    Vertex / Index
    // ========================
    /// <summary>キューブの頂点構造体（位置のみ）。</summary>
    struct CubeVertex
    {
        Vector4 position; ///< 頂点座標（x, y, z, w）。
    };
    Microsoft::WRL::ComPtr<ID3D12Resource> vertexResource_; ///< 頂点バッファ。
    CubeVertex* vertexData_ = nullptr;                      ///< 頂点データのマップ先。
    Microsoft::WRL::ComPtr<ID3D12Resource> indexResource_;  ///< インデックスバッファ。
    uint32_t* indexData_ = nullptr;                         ///< インデックスデータのマップ先。

    // ========================
    //   Transform Matrix CB
    // ========================
    /// <summary>
    /// 座標変換行列用の定数バッファ。  
    /// WVP：ワールド×ビュー×プロジェクション  
    /// World：ワールド行列  
    /// WorldInverseTranspose：法線変換用
    /// </summary>
    struct CubeTransformationMatrix
    {
        Matrix4x4 WVP;
        Matrix4x4 World;
        Matrix4x4 WorldInverseTranspose;
    };
    Microsoft::WRL::ComPtr<ID3D12Resource> transformationMatrixResource_; ///< 行列 CB リソース。
    CubeTransformationMatrix* transformationMatrixData_ = nullptr;         ///< 行列 CB マップ先。

    // ========================
    //       Buffer Views
    // ========================
    D3D12_VERTEX_BUFFER_VIEW vertexBufferView_ = {}; ///< 頂点バッファビュー。
    D3D12_INDEX_BUFFER_VIEW  indexBufferView_ = {}; ///< インデックスバッファビュー。

    // ========================
    //        Texture
    // ========================
    /// <summary>使用するキューブマップの SRV インデックス。</summary>
    uint32_t textureIndex_ = 0;

    /// <summary>使用するキューブマップのテクスチャファイルパス。</summary>
    std::string textureFilePath_;

    // ========================
    //       Copy Control
    // ========================
    /// <summary>コピーコンストラクタ（禁止）。</summary>
    ObjectCubemap(const ObjectCubemap&) = delete;

    /// <summary>代入演算子（禁止）。</summary>
    ObjectCubemap& operator=(const ObjectCubemap&) = delete;
};

} // namespace Graphics
} // namespace TYEngine
