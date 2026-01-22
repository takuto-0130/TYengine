#pragma once
#include <fstream>
#include "struct.h"
#include "Matrix4x4Func.h"
#include "mathFunc.h"
#include "Model.h"
#include "ModelManager.h"
#include "Camera.h"
class Object3dBasis;
class WorldTransform;

enum class Lighting
{
	NONE,
	HALF_LAMBERT,
	LAMBERT,
};

/// <summary>
/// 単一の 3D オブジェクトを描画するためのクラス。  
/// マテリアル、カメラ、モデル、各種 GPU リソースを保持し、描画を実行する。
/// </summary>
class Object3d
{
public: // メンバ関数
    /// <summary>デフォルトコンストラクタ。</summary>
    Object3d() = default;

    /// <summary>デストラクタ。</summary>
    ~Object3d() = default;

    /// <summary>
    /// 初期化処理。  
    /// Object3dBasis の参照取得、マテリアル／カメラ用定数バッファの作成とマッピングを行う。
    /// </summary>
    void Initialize();

    /// <summary>
    /// 描画処理。  
    /// 渡されたワールド変換と、内部のモデル・マテリアル・カメラ情報を用いて描画する。
    /// </summary>
    /// <param name="worldTransform">オブジェクトのワールド変換（WVP 計算に使用）。</param>
    void Draw(WorldTransform& worldTransform);

private:
    /// <summary>
    /// マテリアル用の定数バッファを作成・マップする。
    /// </summary>
    void CreateMaterialResource();

    /// <summary>
    /// カメラ用の定数バッファを作成・マップする。
    /// </summary>
    void CreateCameraResource();

private: // 構造体
    /// <summary>
    /// マテリアル定数バッファ。  
    /// color：RGBA 色、enableLighting：ライティング有効、uvTransform：UV 変換、  
    /// environmentCoefficient：環境マップ反射係数、shininess：鏡面ハイライトの鋭さ。
    /// </summary>
    struct Material
    {
        Vector4 color;             ///< マテリアルカラー（RGBA）
        Vector4 addColor;          ///< 加算合成カラー（RGBA）
        bool   enableLighting;     ///< ライティングの有効フラグ
        float  padding[3];         ///< アライメント用パディング
        Matrix4x4 uvTransform;     ///< UV 変換行列
        float environmentCoefficient; ///< 環境マップ反射係数（0～1）
        float shininess;           ///< 鏡面反射の鋭さ（ハイライト係数）
    };

    /// <summary>
    /// カメラ定数バッファ。  
    /// worldPosition：カメラのワールド座標（スペキュラ等で使用）。
    /// </summary>
    struct CameraForGPU
    {
        Vector3 worldPosition;     ///< カメラのワールド座標
    };

private: // メンバ変数
    Object3dBasis* objectManager_ = nullptr;        ///< 3D 共通基盤（ルートシグネチャ／PSO 等）。
    Model* model_ = nullptr;                ///< 描画対象モデル。
    Camera* camera_ = nullptr;               ///< 使用するカメラ。

    Microsoft::WRL::ComPtr<ID3D12Resource> materialResource_ = nullptr; ///< マテリアル CB リソース。
    Microsoft::WRL::ComPtr<ID3D12Resource> cameraResource_ = nullptr; ///< カメラ CB リソース。

    Material* materialData_ = nullptr;         ///< マテリアル CB のマップ先。
    CameraForGPU* cameraData_ = nullptr;         ///< カメラ CB のマップ先。

    const std::string defaultModelsPath_ = "Resources/Models/"; ///< モデル読み込みの既定ディレクトリ。

    // コピー禁止
    Object3d(const Object3d&) = delete;
    Object3d& operator=(const Object3d&) = delete;

public: // プロパティ操作
    /// <summary>描画に使用するモデルを設定する。</summary>
    /// <param name="model">モデルインスタンス。</param>
    void SetModel(Model* model) { model_ = model; }

    /// <summary>ファイルからモデルを読み込み設定する。</summary>
    /// <param name="filePath">モデルファイルへの相対／絶対パス。</param>
    void SetModel(const std::string& filePath);

    /// <summary>使用するカメラを設定する。</summary>
    /// <param name="camera">カメラインスタンス。</param>
    void SetCamera(Camera* camera) { camera_ = camera; }

    /// <summary>現在のマテリアルカラーを取得する。</summary>
    /// <returns>RGBA カラー。</returns>
    const Vector4& GetColor() const { return materialData_->color; }

    /// <summary>マテリアルカラーを設定する。</summary>
    /// <param name="color">RGBA カラー。</param>
    void SetColor(const Vector4& color) { materialData_->color = color; }

    /// <summary>現在の透明度を取得する。</summary>
    /// <returns>alpha 透明度。</returns>
    const float& GetAlpha() const { return materialData_->color.w; }

    /// <summary>透明度を設定する。</summary>
    /// <param name="alpha">alpha 透明度。</param>
    void SetAlpha(float alpha) { materialData_->color.w = alpha; }

    /// <summary>現在の加算合成カラーを取得する。</summary>
    /// <returns>RGBA カラー。</returns>
    const Vector4& GetAddColor() const { return materialData_->addColor; }

    /// <summary>加算合成カラーを設定する。</summary>
    /// <param name="color">RGBA カラー。</param>
    void SetAddColor(const Vector4& color) { materialData_->addColor = color; }

    /// <summary>ライティング有効フラグを取得する。</summary>
    /// <returns>true でライティング有効。</returns>
    const bool& GetIsLighting() const { return materialData_->enableLighting; }

    /// <summary>ライティングの有効・無効を設定する。</summary>
    /// <param name="isLighting">true で有効、false で無効。</param>
    void SetIsLighting(const bool isLighting) { materialData_->enableLighting = isLighting; }

    /// <summary>環境マップ反射係数を設定する（0～1）。</summary>
    /// <param name="environmentCoefficient">反射係数。</param>
    void SetEnvironmentCoefficient(float environmentCoefficient) { materialData_->environmentCoefficient = environmentCoefficient; }
};

/// <summary>
/// Object3d はコピー代入不可であることをコンパイル時に検証する static_assert。
/// </summary>
static_assert(!std::is_copy_assignable_v<Object3d>);
