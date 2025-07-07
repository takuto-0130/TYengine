#pragma once
#include "Model.h"
#include "ModelManager.h"
#include "Camera.h"
#include "WorldTransform.h"
#include "CubemapBasis.h"

const int indexNum = 36;

class ObjectCubemap
{
public:
    ObjectCubemap() = default;
    ~ObjectCubemap() = default;

    void Initialize(const std::string& textureFilePath);
    void Update();
    void Draw();

    void SetCamera(Camera* camera) { camera_ = camera; }

private:
    void CreateCameraResource();

    // 頂点データ作成
    void CreateVertexData();

    // マテリアルリソース作成
    void CreateMaterialResource();

    // 座標変換行列リソース作成
    void CreateTransformationMatrixResource();

private:
    Camera* camera_ = nullptr;


    // カメラ座標
    struct CameraForGPU {
        Vector3 worldPosition;
    };
    Microsoft::WRL::ComPtr<ID3D12Resource> cameraResource_;
    CameraForGPU* cameraData_ = nullptr;


    // マテリアルデータ
    struct CubeMaterial {
        Vector4 color;
    };
    Microsoft::WRL::ComPtr<ID3D12Resource> materialResource_;
    CubeMaterial* materialData_ = nullptr;


    // 頂点データ
    struct CubeVertex
    {
        Vector4 position;
    };
    Microsoft::WRL::ComPtr<ID3D12Resource> vertexResource_;
    CubeVertex* vertexData_ = nullptr;
    Microsoft::WRL::ComPtr<ID3D12Resource> indexResource_;
    uint32_t* indexData_ = nullptr;
    

    // 座標変換行列データ
    struct CubeTransfomationMatrix {
        Matrix4x4 WVP;
        Matrix4x4 World;
        Matrix4x4 WorldInverseTranspose;
    };
    Microsoft::WRL::ComPtr<ID3D12Resource> transformationMatrixResource_;
    CubeTransfomationMatrix* transformationMatrixData_ = nullptr;


    D3D12_VERTEX_BUFFER_VIEW vertexBufferView_;
    D3D12_INDEX_BUFFER_VIEW indexBufferView_;


    // テクスチャ番号
    uint32_t textureIndex_ = 0;
    std::string textureFilePath_;


    // コピー禁止
    ObjectCubemap(const ObjectCubemap&) = delete;
    ObjectCubemap& operator=(const ObjectCubemap&) = delete;
};

