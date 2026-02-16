#pragma once
#include "IScene.h"
#include "ObjectCubemap.h"
#include "Object/Player/Player.h"
#include "../../Object/Enemy/EnemyManager/EnemyManager.h"

#include "AudioAnalyzer.h"
#include "BeatAnalyzer.h"

#define JSONMGR_WITH_IMGUI
#include "Utils/Json/JsonManager.h"

#include "Sprite.h"

class GameAudio;

/// <summary>
/// タイトル画面シーン。
/// ゲーム開始前の演出、操作説明、ゲーム本編への遷移を管理する。
/// </summary>
class TitleScene : public TYEngine::Framework::IScene {
public:
	~TitleScene();

	/// <summary>
	/// 初期化処理。
	/// タイトルロゴ、背景、デモプレイ用キャラなどの生成を行う。
	/// </summary>
	void Init() override;

	/// <summary>
	/// 更新処理。
	/// 入力受付やシーン遷移制御を行う。
	/// </summary>
	void Update() override;

	/// <summary>
	/// 3Dオブジェクト等の描画。
	/// </summary>
	void Draw() override;

	/// <summary>
	/// UI（スプライト）描画処理。
	/// </summary>
	void UIDraw() override;

private:
	/// <summary>
	/// シーン遷移処理。ゲーム本編へ切り替える。
	/// </summary>
	void Transition();

	/// <summary>JSONパラメータの反映（デバッグ用）。</summary>
	void DebugJMApply();

private:
	/// <summary>スペース（Press Space）表示スプライト。</summary>
	std::unique_ptr<TYEngine::Graphics::Sprite> spaceSpr_;
	/// <summary>タイトルテキストスプライト。</summary>
	std::unique_ptr<TYEngine::Graphics::Sprite> text_;
	/// <summary>操作説明スプライト。</summary>
	std::unique_ptr<TYEngine::Graphics::Sprite> operation_;
	/// <summary>レティクルスプライト（演出用）。</summary>
	std::unique_ptr<TYEngine::Graphics::Sprite> reticle_;

	/// <summary>スカイボックス。</summary>
	std::unique_ptr<TYEngine::Graphics::ObjectCubemap> skybox_;

	/// <summary>デモ用プレイヤー。</summary>
	std::unique_ptr<Player> player_;

	/// <summary>デモ用敵マネージャ。</summary>
	EnemyManager enemyMgr_;


	/// <summary>背景（地面）。</summary>
	std::unique_ptr<TYEngine::Graphics::Object3d> ground_;
	/// <summary>背景のワールド変換情報。</summary>
	TYEngine::Utility::WorldTransform groundWT_;
	/// <summary>背景の回転速度。</summary>
	float rotateSpeed_ = 0.0f;

	/// <summary>JSONマネージャ（デバッグ設定用）。</summary>
	TYEngine::Utility::JsonManager titleJM;
	/// <summary>JSONエラーメッセージ。</summary>
	std::string err;

	/// <summary>オーディオスペクトラムアナライザー（演出用）。</summary>
	TYEngine::AudioSystem::AudioAnalyzer audioAnalyzer_;

	/// <summary>ビートアナライザー（演出用）。</summary>
	TYEngine::AudioSystem::BeatAnalyzer beatAnalyzer_;

	int bgmHandle_ = -1;

	GameAudio* gameAudio_ = nullptr;
};
