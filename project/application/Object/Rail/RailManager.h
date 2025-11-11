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

// 弧長テーブル（ポリライン用）
struct PolylineArc
{
	std::vector<float> S; // 累積距離
	std::vector<float> T; // そのサンプルのt
	float total = 0.0f;   // 総延長
};

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

	float GetRailTotalLength() const { return arcMap_.total; }

	bool RailTrigger();

	bool IsEndRail() { return cameraForwardT >= 1.0f ? true : false; }

	bool ClearEnemyGroup()
	{
		return (cameraEyeT >= 0.25f && !firstClear_) || (cameraEyeT >= 0.83f && !secondClear_);
	}

	void ClearAccept()
	{
		if (!firstClear_)
		{
			firstClear_ = true;
		}
		else
		{
			secondClear_ = true;
		}
	}

private:
	void PopRail(Vector3 position, Vector3 rota);

	void RailReDraw();

	void RailCameraDebug();

	void SetSegment();

	void ResetRailCamera();

	static PolylineArc BuildPolylineArc(const std::vector<Vector3>& poly)
	{
		PolylineArc map;

		const size_t N = poly.size();
		if (N == 0) {
			map.S = {};
			map.T = {};
			map.total = 0.0f;
			return map;
		}
		if (N == 1) {
			map.S = { 0.0f };
			map.T = { 0.0f };
			map.total = 0.0f;
			return map;
		}

		map.S.resize(N);
		map.T.resize(N);

		map.S[0] = 0.0f;
		map.T[0] = 0.0f;

		for (size_t i = 1; i < N; ++i) {
			map.S[i] = map.S[i - 1] + Length(poly[i - 1] - poly[i]);
			map.T[i] = static_cast<float>(i) / static_cast<float>(N - 1); // 等間隔 t
		}

		map.total = map.S.back();
		return map;
	}

	static float DistanceToT_Hybrid(const PolylineArc& map, float s)
	{
		if (map.S.empty())  return 0.0f;
		if (s <= 0.0f)      return 0.0f;
		if (s >= map.total) return 1.0f;

		size_t lo = 0, hi = map.S.size() - 1;
		while (hi - lo > 1) {
			const size_t mid = (lo + hi) / 2;
			if (map.S[mid] <= s) lo = mid; else hi = mid;
		}

		const float s0 = map.S[lo];
		const float s1 = map.S[lo + 1];
		const float t0 = map.T[lo];
		const float t1 = map.T[lo + 1];

		const float a = (s - s0) / std::max<float>(1e-6f, (s1 - s0));
		return Lerp(t0, t1, a); // 線形補間
	}

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
	float cameraSegmentCount = 0;
	float cameraEyeT = 0;
	float cameraForwardT = 0;
	bool isRailCameraMove_ = false;

	float deltaTime_ = 1.0f / 60.0f;

	float speedMultiply_ = 0.7f;


	bool firstClear_ = false;
	bool secondClear_ = false;


	std::unordered_set<size_t> alreadyTriggeredIndices_;
	struct TriggerObject
	{
		WorldTransform world;
		Object3d object;

		explicit TriggerObject(const Vector3& pos)
		{
			world.Initialize();
			world.translation_ = pos;
			world.colliderScale_ = { 0.5f, 0.5f, 0.5f };
			object.Initialize();
			object.SetModel("unitSphere.obj");
		}

		TriggerObject(const TriggerObject&) = delete;
		TriggerObject& operator=(const TriggerObject&) = delete;

		TriggerObject(TriggerObject&&) noexcept = default;
		TriggerObject& operator=(TriggerObject&&) noexcept = default;
	};
	std::vector<std::unique_ptr<TriggerObject>> triggerObjects_;

	// トリガーの弧長位置（s）と発火済みフラグ
	std::vector<float> triggerS_;
	std::vector<bool>  triggerFired_;
	// 前フレームの弧長
	float prevEyeS_ = 0.0f;
	// 制御点→弧長へ投影するヘルパ
	void RebuildTriggerSFromSegments();


	PolylineArc arcMap_;
	float eyeS_ = 0.0f;
	float forwardS_ = 0.0f;
	float lookAhead_ = 1.0f;   // 先読み距離
	float speedMps_ = 5.0f;   // 速度
};

