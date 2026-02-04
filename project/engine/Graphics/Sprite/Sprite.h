#pragma once
#include "struct.h"
#include <wrl.h>
#include <Windows.h>
#include <array>
#include <d3d12.h>

// class SpriteBasis; // Moved to inside namespace or remove if unused here? It is used.

namespace TYEngine {
namespace Graphics {

// using namespace Utility::SceneObjects; // For Transform
class SpriteBasis; // Forward declaration inside namespace

/// <summary>
/// 2D スプライト描画を管理・実行するクラス。  
/// テクスチャ、サイズ、カラー、アンカーポイント、フリップなどを制御し、  
/// シンプルな矩形スプライトを画面上に描画する。
/// </summary>
class Sprite
{
public: // メンバ関数
    /// <summary>
    /// スプライトを初期化する。  
    /// 指定したテクスチャを読み込み、頂点・マテリアル・行列バッファを生成する。
    /// </summary>
    /// <param name="textureFilePath">使用するテクスチャファイルのパス。</param>
    void Initialize(std::string textureFilePath);

    /// <summary>
    /// スプライトの毎フレーム更新処理。  
    /// 座標変換や UV 設定の反映を行う。
    /// </summary>
    void Update();

    /// <summary>
    /// スプライトを描画する。  
    /// 現在の設定（位置・回転・色など）に基づいて描画を行う。
    /// </summary>
    void Draw();

    /// <summary>
    /// 4 頂点を明示的に指定してスプライトを描画する。
    /// </summary>
    /// <param name="lt">左上頂点座標。</param>
    /// <param name="rt">右上頂点座標。</param>
    /// <param name="lb">左下頂点座標。</param>
    /// <param name="rb">右下頂点座標。</param>
    void DrawRect(const ::TYEngine::Utility::Vector2& lt, const ::TYEngine::Utility::Vector2& rt, const ::TYEngine::Utility::Vector2& lb, const ::TYEngine::Utility::Vector2& rb);

    // ========================
    //         Getter
    // ========================

    /// <summary>座標を取得する。</summary>
    const ::TYEngine::Utility::Vector2& GetPosition() { return position_; }
    /// <summary>回転角（ラジアン）を取得する。</summary>
    float GetRotation() const { return rotation_; }
    /// <summary>色（RGBA）を取得する。</summary>
    const ::TYEngine::Utility::Vector4& GetColor() { return materialData_->color; }
    /// <summary>サイズ（幅・高さ）を取得する。</summary>
    const ::TYEngine::Utility::Vector2& GetSize() { return size_; }
    /// <summary>テクスチャ切り出し位置（左上）を取得する。</summary>
    const ::TYEngine::Utility::Vector2& GetTextureLeftTop() { return textureLeftTop_; }
    /// <summary>テクスチャ切り出しサイズを取得する。</summary>
    const ::TYEngine::Utility::Vector2& GetTextureSize() { return textureSize_; }
    /// <summary>アンカーポイント（中心点）を取得する。</summary>
    const ::TYEngine::Utility::Vector2& GetAnchorPoint() const { return anchorPoint_; }
    /// <summary>左右反転フラグを取得する。</summary>
    const bool& GetIsFlipX() const { return isFlipX_; }
    /// <summary>上下反転フラグを取得する。</summary>
    const bool& GetIsFlipY() const { return isFlipY_; }

    // ========================
    //         Setter
    // ========================

    /// <summary>座標を設定する。</summary>
    void SetPosition(const ::TYEngine::Utility::Vector2& position) { position_ = position; }
    /// <summary>回転角（ラジアン）を設定する。</summary>
    void SetRotation(const float& rotation) { rotation_ = rotation; }
    /// <summary>色（RGBA）を設定する。</summary>
    void SetColor(const ::TYEngine::Utility::Vector4& color) { materialData_->color = color; }
    /// <summary>透明度（Alpha）を設定する。</summary>
    void SetAlpha(const float& alpha) { materialData_->color.w = alpha; }
    /// <summary>サイズ（幅・高さ）を設定する。</summary>
    void SetSize(const ::TYEngine::Utility::Vector2& size) { size_ = size; }
    /// <summary>テクスチャ切り出し位置（左上）を設定する。</summary>
    void SetTextureLeftTop(::TYEngine::Utility::Vector2 textureLeftTop) { textureLeftTop_ = textureLeftTop; }
    /// <summary>テクスチャ切り出しサイズを設定する。</summary>
    void SetTextureSize(::TYEngine::Utility::Vector2 textureSize) { textureSize_ = textureSize; }
    /// <summary>アンカーポイント（中心点）を設定する。</summary>
    void SetAnchorPoint(const ::TYEngine::Utility::Vector2& anchorPoint) { anchorPoint_ = anchorPoint; }
    /// <summary>左右反転フラグを設定する。</summary>
    void SetIsFlipX(bool isFlipX) { isFlipX_ = isFlipX; }
    /// <summary>上下反転フラグを設定する。</summary>
    void SetIsFlipY(bool isFlipY) { isFlipY_ = isFlipY; }

    /// <summary>
    /// 頂点位置を個別に設定する。
    /// </summary>
    /// <param name="position">頂点座標。</param>
    /// <param name="index">設定する頂点インデックス。</param>
    void SetVertex(const ::TYEngine::Utility::Vector2& position, int index) { vertexData_[index].position = { position.x, position.y, 0.0f, 1.0f }; }

    /// <summary>
    /// スプライトの頂点情報や UV、サイズを GPU バッファへ反映する。
    /// </summary>
    void SetSpriteData();

private: // メンバ関数
    /// <summary>
    /// 頂点データの初期化とバッファ作成を行う。
    /// </summary>
    void CreateVertexData();

    /// <summary>
    /// マテリアル用の GPU リソースを作成する。
    /// </summary>
    void CreateMaterialResource();

    /// <summary>
    /// 座標変換行列用の GPU リソースを作成する。
    /// </summary>
    void CreateTransformationMatrixResource();

    /// <summary>
    /// テクスチャサイズとスプライト矩形を調整する。
    /// </summary>
    void AdjustTextureSize();

private: // メンバ変数
    SpriteBasis* spriteBasis_ = nullptr; ///< スプライト描画共通設定への参照。

    /// <summary>頂点データ構造体。</summary>
    struct VertexData
    {
        ::TYEngine::Utility::Vector4 position; ///< 頂点座標（x, y, z, w）。
        ::TYEngine::Utility::Vector2 texCoord; ///< UV 座標。
    };

    /// <summary>マテリアルデータ構造体。</summary>
    struct Material
    {
        ::TYEngine::Utility::Vector4 color;          ///< RGBA カラー。
        ::TYEngine::Utility::Matrix4x4 uvTransform;  ///< UV 変換行列。
    };

    /// <summary>座標変換行列データ構造体。</summary>
    struct TransformationMatrix
    {
        ::TYEngine::Utility::Matrix4x4 WVP; ///< ワールド × ビュー × プロジェクション行列。
    };

    // ========================
    //     GPU リソース関連
    // ========================

    Microsoft::WRL::ComPtr<ID3D12Resource> indexResource_;             ///< インデックスバッファ。
    Microsoft::WRL::ComPtr<ID3D12Resource> vertexResource_;            ///< 頂点バッファ。
    Microsoft::WRL::ComPtr<ID3D12Resource> materialResource_;          ///< マテリアル定数バッファ。
    Microsoft::WRL::ComPtr<ID3D12Resource> transformationMatrixResource_; ///< 行列定数バッファ。

    uint32_t* indexData_ = nullptr;                 ///< インデックスデータのマップポインタ。
    VertexData* vertexData_ = nullptr;              ///< 頂点データのマップポインタ。
    Material* materialData_ = nullptr;              ///< マテリアルデータのマップポインタ。
    TransformationMatrix* transformationMatrixData_ = nullptr; ///< 行列データのマップポインタ。

    D3D12_INDEX_BUFFER_VIEW indexBufferView_;       ///< インデックスバッファビュー。
    D3D12_VERTEX_BUFFER_VIEW vertexBufferView_;     ///< 頂点バッファビュー。

    // ========================
    //        スプライト情報
    // ========================

    uint32_t textureIndex_ = 0;                      ///< 使用テクスチャの SRV インデックス。
    std::string textureFilePath_;                   ///< 使用中のテクスチャファイルパス。

    bool isFlipX_ = false; ///< 左右反転フラグ。
    bool isFlipY_ = false; ///< 上下反転フラグ。

    ::TYEngine::Utility::Transform transform_{ {1.0f, 1.0f, 1.0f}, {0.0f, 0.0f, 0.0f}, {0.0f, 0.0f, 0.0f} }; ///< 3D変換情報。
    ::TYEngine::Utility::Vector2 position_ = { 0.0f, 0.0f }; ///< スプライトの描画位置。
    float rotation_ = 0.0f;             ///< 回転角度（ラジアン）。
    ::TYEngine::Utility::Vector2 size_ = { 100.0f, 100.0f }; ///< スプライトのサイズ。

    ::TYEngine::Utility::Vector2 anchorPoint_ = { 0.0f, 0.0f }; ///< 原点（アンカー）位置。

    ::TYEngine::Utility::Vector2 textureLeftTop_ = { 0.0f, 0.0f }; ///< テクスチャの左上 UV。
    ::TYEngine::Utility::Vector2 textureSize_ = { 100.0f, 100.0f }; ///< 使用するテクスチャ範囲のサイズ。
};

} // namespace Graphics
} // namespace TYEngine
