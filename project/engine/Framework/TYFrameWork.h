#pragma once
#include "WindowsApp.h"
#include <cassert>
#include <dxgidebug.h>
#include <dxcapi.h>
#include "DirectXTex/DirectXTex.h"
#include "DirectXTex/d3dx12.h"
#include <fstream>
#include <sstream>
#include <wrl.h>
#include "DirectXBasis.h"
#include "D3DResourceLeakChecker.h"
#include "SpriteBasis.h"
#include "TextureManager.h"
#include "SrvManager.h"
#include "ModelManager.h"
#include "Object3dBasis.h"
#include "ImGuiManager.h"
#include "Input.h"
#include "SceneManager.h"
#include "AbstractSceneFactory.h"
#include "CopyPass.h"
#include "RenderTexture.h"
#include "PostEffectManager.h"
#ifdef _DEBUG
#include <imgui.h>
#endif // _DEBUG

#include <xaudio2.h>
#pragma comment(lib,"xaudio2.lib")

/// <summary>
/// エンジン全体の実行サイクルを管理するフレームワーク基底クラス。  
/// 各種マネージャ（DirectX・描画・入力・シーンなど）を統括し、  
/// ゲームアプリケーションのライフサイクルを制御する。
/// </summary>
class TYFrameWork
{
public:
    /// <summary>仮想デストラクタ。</summary>
    virtual ~TYFrameWork() = default;

    /// <summary>
    /// メインループを実行する。  
    /// Initialize → Update → Draw → Finalize の順に処理を行う。
    /// </summary>
    void run();

    /// <summary>
    /// 初期化処理。  
    /// 各マネージャや基盤システムを構築する。
    /// </summary>
    virtual void Initialize();

    /// <summary>
    /// 終了処理。  
    /// 使用したリソースやインスタンスを解放する。
    /// </summary>
    virtual void Finalize();

    /// <summary>
    /// 毎フレーム更新処理。  
    /// 入力、シーン遷移、ポストエフェクトなどを更新する。
    /// </summary>
    virtual void Update();

    /// <summary>
    /// 描画処理（純粋仮想関数）。  
    /// 派生クラス側で具体的な描画を実装する。
    /// </summary>
    virtual void Draw() = 0;

    /// <summary>
    /// 終了リクエストフラグを取得する。
    /// </summary>
    /// <returns>終了要求が出ていれば true。</returns>
    virtual bool IsEndRequest() { return endRequest_; };

protected:
#ifdef _DEBUG
    /// <summary>
    /// DirectX リソースリーク検出用（デバッグビルド専用）。
    /// </summary>
    std::unique_ptr<D3DResourceLeakChecker> leakCheck;
#endif // _DEBUG

    /// <summary>Windows アプリケーション管理クラス。</summary>
    std::unique_ptr<WindowsApp> windowsApp = nullptr;

    /// <summary>DirectX の基盤管理クラス。</summary>
    DirectXBasis* directXBasis = nullptr;

    /// <summary>SRV 管理クラス。</summary>
    std::unique_ptr<SrvManager> srvManager = nullptr;

    /// <summary>2D スプライト描画の基盤クラス。</summary>
    SpriteBasis* spriteBasis = nullptr;

    /// <summary>3D オブジェクト描画の基盤クラス。</summary>
    Object3dBasis* object3dBasis = nullptr;

    /// <summary>3D モデル管理クラス。</summary>
    ModelManager* modelManager = nullptr;

    /// <summary>入力（キーボード・マウス・パッド）管理クラス。</summary>
    Input* input = nullptr;

    /// <summary>ImGui デバッグ UI 管理クラス。</summary>
    ImGuiManager* imgui = nullptr;

    /// <summary>カメラクラス。</summary>
    std::unique_ptr<Camera> camera = nullptr;

    /// <summary>シーン管理クラス。</summary>
    SceneManager* sceneManager_ = nullptr;

    /// <summary>シーン生成ファクトリ。</summary>
    std::unique_ptr<AbstractSceneFactory> sceneFactory_ = nullptr;

    /// <summary>アプリケーション終了要求フラグ。</summary>
    bool endRequest_ = false;

    /// <summary>メインレンダーターゲット。</summary>
    std::unique_ptr<RenderTexture> renderTexture;

    /// <summary>一時的なレンダーターゲット（ping-pong 用）。</summary>
    std::unique_ptr<RenderTexture> tempTexture;

    /// <summary>ポストエフェクト管理クラス。</summary>
    PostEffectManager* postEffectManager;

    /// <summary>アウトライン描画用のレンダーターゲット。</summary>
    std::unique_ptr<RenderTexture> outlineTexture;
};
