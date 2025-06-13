#pragma once

#include "IScene.h"
#include "Sprite.h"
#include "Object3d.h"
#include "WorldTransform.h"
#include <sstream>
#include "Audio/Audio.h"
#include "Rail/Rail.h"
#include "Object/Enemy/Enemy.h"
#include "Rail/RailEditor.h"
#include "Object/Enemy/EnemyEditor.h"
#include "Skydome/Skydome.h"
#include "ParticleManager.h"
#include "./Score/score.h"


#include <memory>
#include <vector>
#include <list>
#include <unordered_set>

/// <summary>
/// ゲームシーン
/// </summary>
class GameScene : public IScene
{
public:
	~GameScene() override;
	void Init() override;
	void Update() override;
	void Draw() override;
	void CheckAllCollisions();

private:
	void PopRail(Vector3 position, Vector3 rota);
	void RailCustom();
	void RailLineReDraw();
	void RailReDraw();
	void RailCameraMove();
	void RailCameraDebug();
	void SetSegment();
	void ResetRailCamera();

	void TriggerNextEnemyGroup();

	void Collision();

	std::list<std::list<std::unique_ptr<Enemy>>> DeepCopyEnemyGroups(const std::list<std::list<std::unique_ptr<Enemy>>>& src);

#ifdef _DEBUG
	void DrawEditorEnemies();
	void UpdateEditorEnemies();
#endif // _DEBUG


private:
	float pitch_ = 1.0f;
	Vector3 cameraOffset_;

#ifdef _DEBUG
	bool isEffect_ = false;
#endif
	std::unique_ptr<Skydome> skydome_;
	std::list<std::unique_ptr<Rail>> rails_;

	std::list<std::list<std::unique_ptr<Enemy>>> enemyGroupsEditor_; // 編集用
	std::list<std::list<std::unique_ptr<Enemy>>> enemyGroups_; // 全グループ（未出現）
	std::list<std::unique_ptr<Enemy>> activeEnemies_;          // 今出現中の敵
	std::unique_ptr<EnemyEditor> enemyEditor_;				   // 敵のエディタ

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


	IParticleRenderer::Emitter emitter;


	IParticleRenderer::Emitter emitterRing;


	std::unique_ptr<score> scoreDraw_;
	int32_t score_ = 0;
	const int32_t kBasicScore_ = 200;

	std::unique_ptr<Sprite> reticle_;

	std::array<std::unique_ptr<Sprite>, 2> lasers_;

	bool showEditorEnemies = false;

	std::unique_ptr<Sprite> comboText_;
	Vector2 offsetPos_ = { 1245, 60 };

	std::unique_ptr<Sprite> one_;
	Vector2 offsetNum_ = { 1060,25 };

	std::random_device seedGene_;
	float shakeTime_ = 0.4f;
};
