#pragma once
#include "Object3d.h"
#include "WorldTransform.h"
#include "Rail.h"

#include <memory>
#include <vector>
#include <list>
#include <unordered_set>

class RailEditor;
class Camera;

class RailManager
{
public:
	void Init();

	void Update();

	void Draw();

	void UpdateEdit();

	void StageEdit();

	void RailCameraMove();

	void Reset();

public:
	void SetCamera(Camera* camera) { camera_ = camera; }

	bool RailTrigger();

	bool IsEndRail() { return cameraForwardT >= 1.0f ? true : false; }

private:
	void PopRail(Vector3 position, Vector3 rota);

	void RailReDraw();

	void RailCameraDebug();

	void SetSegment();

	void ResetRailCamera();

private:
	Camera* camera_ = nullptr;

	Vector3 offsetCameraPos_ = { 0.0f,0.8f,0.0f };

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
};

