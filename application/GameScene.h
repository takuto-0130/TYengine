#pragma once

#include "IScene.h"
#include "Input.h"
#include "Model.h"
#include "Sprite.h"
#include "Object3d.h"
#include "WorldTransform.h"
#include <sstream>
#include "../engine/Audio/Audio.h"
#include "Rail/Rail.h"
#include "Rail/RailEditor.h"
#include "Skydome/Skydome.h"
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

private:
	float pitch_ = 1.0f;
	Vector3 cameraOffset_;

#ifdef _DEBUG
	bool isEffect_ = false;
#endif

	WorldTransform worldTransform_;
	std::unique_ptr<Object3d> obj_;
	std::unique_ptr<Skydome> skydome_;
	std::list<std::unique_ptr<Rail>> rails_;

	std::vector<bool> triggeredFlags_;
	std::vector<Vector3> controlPoints_;
	std::vector<Vector3> pointsDrawing_;
	size_t oneSegmentCount = 20;
	size_t segmentCount = oneSegmentCount;
	const float kDivisionSpan = 100.0f;
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
			object.SetModel("cube.obj");
		}

		TriggerObject(const TriggerObject&) = delete;
		TriggerObject& operator=(const TriggerObject&) = delete;

		TriggerObject(TriggerObject&&) noexcept = default;
		TriggerObject& operator=(TriggerObject&&) noexcept = default;
	};

	std::vector<std::unique_ptr<TriggerObject>> triggerObjects_;
};
