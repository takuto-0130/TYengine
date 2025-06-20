#pragma once

#include "IScene.h"
#include "Sprite.h"
#include "Object3d.h"
#include "WorldTransform.h"
#include "./Score/score.h"
#include "StateMachine.h"

#include "Rail/Rail.h"
#include "Rail/RailEditor.h"
#include "Skydome/Skydome.h"
#include "ParticleManager.h"
#include "../../Object/Enemy/EnemyManager.h"


#include <memory>
#include <vector>
#include <list>
#include <unordered_set>

class Pause;
class Result;
class Audio;

enum class GameSceneState {
	LOAD,		// データのロード（ロードに時間がかかる場合に使用、使わなければのちに削除）
	FADE_IN,	// シーン開始時のフェードイン
	READY,		// ゲーム開始直前の演出（不要なら消す）
	PLAY,		// 通常のゲーム進行
	PAUSE,		// 一時停止状態（メニューなど）
	DEAD,		// プレイヤーが死んだ時
	RESULT,		// リザルト（ゲーム終了後のスコア表示など）
	RETRY,		// リトライ待機中
	FADE_OUT,	// シーン遷移時のフェードアウト
	DEBUG_EDIT,	// エディタ専用
};

/// <summary>
/// ゲームシーン
/// </summary>
class GameScene : public IScene, public StateMachine<GameScene, GameSceneState>
{
public: // 関数テーブル
	static const std::vector<StateFunctionSet>& GetStateTable();

public:
	GameScene();
	~GameScene() override;
	void Init() override;
	void Update() override;
	void Draw() override;

private:
	void PlayUIUpdate();

	void AttackUpdate();

	void PopRail(Vector3 position, Vector3 rota);
	void StageEdit();
	void RailReDraw();
	void RailCameraMove();
	void RailCameraDebug();
	void SetSegment();
	void ResetRailCamera();

	void Collision();


private: // シーン内のState関連関数
#pragma region // State関連関数
	// 列挙名を文字列化（ImGui表示用）
	std::string GetStateName(State state) const override {
		switch (state) {
		case State::LOAD: return "LOAD";
		case State::FADE_IN: return "FADE_IN";
		case State::READY: return "READY";
		case State::PLAY: return "PLAY";
		case State::PAUSE: return "PAUSE";
		case State::DEAD: return "DEAD";
		case State::RESULT: return "RESULT";
		case State::RETRY: return "RETRY";
		case State::FADE_OUT: return "FADE_OUT";
		case State::DEBUG_EDIT: return "DEBUG_EDIT";
		default: return "Unknown";
		}
	}

	// ロード
	void InitLoad();
	void UpdateLoad();
	void ExitLoad();

	// フェードイン
	void InitFadeIn();
	void UpdateFadeIn();
	void ExitFadeIn();

	// ゲーム開始直前の演出
	void InitReady();
	void UpdateReady();
	void ExitReady();

	// 通常のゲーム進行
	void InitPlay();
	void UpdatePlay();
	void ExitPlay();

	// 一時停止状態（メニューなど）
	void InitPause();
	void UpdatePause();
	void ExitPause();

	// プレイヤーが死んだ時
	void InitDead();
	void UpdateDead();
	void ExitDead();

	// リザルト
	void InitResult();
	void UpdateResult();
	void ExitResult();

	// リトライ待機中
	void InitRetry();
	void UpdateRetry();
	void ExitRetry();

	// フェードアウト
	void InitFadeOut();
	void UpdateFadeOut();
	void ExitFadeOut();

	// エディタ専用
	void InitDebugEdit();
	void UpdateDebugEdit();
	void ExitDebugEdit();
#pragma endregion

private: // メンバ変数

	float pitch_ = 1.0f;
	Vector3 cameraOffset_ = {};

	std::unique_ptr<Skydome> skydome_;

	std::unique_ptr<EnemyManager> enemyManager_;

	std::list<std::unique_ptr<Rail>> rails_;
	std::vector<bool> triggeredFlags_;
	std::vector<Vector3> controlPoints_;
	std::vector<Vector3> pointsDrawing_;
	size_t oneSegmentCount = 20;
	size_t segmentCount = oneSegmentCount;
	const float kDivisionSpan = 200.0f;
	float cameraSegmentCount = 1.0f / 600.0f;
	float cameraEyeT = 0;
	float cameraForwardT = 30.0f / 600.0f;
	bool isRailCameraMove_ = false;

	std::unordered_set<size_t> alreadyTriggeredIndices_;
	struct TriggerObject
	{
		WorldTransform world;
		Object3d object;

		explicit TriggerObject(const Vector3& pos)
		{
			world.Initialize();
			world.translation_ = pos;
			world.scale_ = { 0.5f, 0.5f, 0.5f };
			object.Initialize();
			object.SetModel("unitSphere.obj");
		}

		TriggerObject(const TriggerObject&) = delete;
		TriggerObject& operator=(const TriggerObject&) = delete;

		TriggerObject(TriggerObject&&) noexcept = default;
		TriggerObject& operator=(TriggerObject&&) noexcept = default;
	};
	std::vector<std::unique_ptr<TriggerObject>> triggerObjects_;


	int comboCount_ = 0;
	float comboTimer_ = 0;
	float kComboTime_ = 3.0f;
	std::unique_ptr<Sprite> comboNumTex_;
	Vector2 offsetComboNum_ = { 1060,25 };
	std::unique_ptr<Sprite> comboText_;
	Vector2 offsetComboTextPos_ = { 1245, 60 };


	IParticleRenderer::Emitter emitter;
	IParticleRenderer::Emitter emitterRing;


	std::unique_ptr<Sprite> reticle_;
	std::array<std::unique_ptr<Sprite>, 2> lasers_;


	std::unique_ptr<score> scoreDraw_;
	int32_t score_ = 0;
	const int32_t kBasicScore_ = 200;
	std::unique_ptr<Pause> pauseMenu_;
	std::unique_ptr<Result> resultMenu_;

	bool otherEditorSwitch_ = false;

	std::random_device seedGene_;
	float shakeTime_ = 0.4f;
};
