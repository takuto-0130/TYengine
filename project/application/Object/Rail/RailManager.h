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

/// <summary>
/// ポリラインの弧長パラメータ化テーブル。
/// 距離 s とパラメータ t の変換に使用する。
/// </summary>
struct PolylineArc
{
	std::vector<float> S; ///< 累積距離リスト
	std::vector<float> T; ///< 各点のパラメータ t (0.0～1.0正規化など)
	float total = 0.0f;   ///< ポリラインの総延長
};

/// <summary>
/// レール（軌道）を管理するクラス。
/// レールエディタ、カメラパス制御、イベントトリガーの発火などを統括する。
/// </summary>
class RailManager
{
public:
	/// <summary>
	/// 初期化処理。
	/// レールデータの読み込みや初期設定を行う。
	/// </summary>
	void Init();

	/// <summary>
	/// 毎フレームの更新処理。
	/// レール上の進行、カメラ位置更新、エディタ操作などを処理する。
	/// </summary>
	void Update();

	/// <summary>
	/// 描画処理。
	/// レールのラインや制御点の可視化を行う（デバッグ用含む）。
	/// </summary>
	void Draw();

	/// <summary>
	/// レールエディタ用の更新処理。
	/// </summary>
	void UpdateEdit();

	/// <summary>
	/// ステージ編集情報の更新。
	/// </summary>
	void StageEdit();

	/// <summary>
	/// レールに沿ったカメラ移動処理。
	/// </summary>
	void RailCameraMove();

	/// <summary>
	/// レール状態のリセット。
	/// </summary>
	void Reset();

public:
	void SetCamera(Camera* camera) { camera_ = camera; }

	float GetRailTotalLength() const { return arcMap_.total; }

	bool RailTrigger();

	bool IsEndRail() { return railFinished_; }

private:
	/// <summary>レールポイントを追加する。</summary>
	void PopRail(const Vector3& position, const Vector3& rotate);

	/// <summary>レールを描画用に再構築する。</summary>
	void RailReDraw();

	/// <summary>レールカメラのデバッグ描画・GUI。</summary>
	void RailCameraDebug();

	/// <summary>セグメント設定を適用する。</summary>
	void SetSegment();

	/// <summary>レールカメラ位置をリセットする。</summary>
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
	/// <summary>カメラへのポインタ。</summary>
	Camera* camera_ = nullptr;

	/// <summary>カメラのオフセット位置。</summary>
	Vector3 offsetCameraPos_ = { 0.0f,0.8f,0.0f };

	/// <summary>レールセグメントのリスト。</summary>
	std::list<std::unique_ptr<Rail>> rails_;
	/// <summary>セグメントごとのトリガーフラグ。</summary>
	std::vector<bool> triggeredFlags_;
	/// <summary>制御点リスト。</summary>
	std::vector<Vector3> controlPoints_;
	/// <summary>描画用ポイントリスト。</summary>
	std::vector<Vector3> pointsDrawing_;

	/// <summary>1セグメントあたりの描画分割数（基準）。</summary>
	size_t oneSegmentCount = 20;
	/// <summary>総セグメント分割数。</summary>
	size_t segmentCount = oneSegmentCount;

	/// <summary>レールカメラ移動中フラグ。</summary>
	bool isRailCameraMove_ = false;

	/// <summary>デルタタイム。</summary>
	float deltaTime_ = 0.0f;

	/// <summary>速度倍率。</summary>
	float speedMultiply_ = 0.35f;


	/// <summary>初回クリアフラグ。</summary>
	bool firstClear_ = false;
	/// <summary>2回目クリアフラグ。</summary>
	bool secondClear_ = false;

	/// <summary>レール終端到達フラグ。</summary>
	bool railFinished_ = false;        
	/// <summary>今フレームで終端に到達したか。</summary>
	bool railFinishedJustNow_ = false; 

	/// <summary>トリガー可視化用オブジェクト。</summary>
	struct TriggerObject
	{
		/// <summary>ワールドトランスフォーム。</summary>
		WorldTransform world;
		/// <summary>描画オブジェクト。</summary>
		Object3d object;

		explicit TriggerObject(const Vector3& pos)
		{
			world.Initialize();
			world.SetTranslation(pos);
			world.SetScale({ 0.1f, 0.1f, 0.1f });
			object.Initialize();
			object.SetColor({ 1,0,0,1 });
			object.SetModel("unitSphere.obj");
		}

		TriggerObject(const TriggerObject&) = delete;
		TriggerObject& operator=(const TriggerObject&) = delete;

		TriggerObject(TriggerObject&&) noexcept = default;
		TriggerObject& operator=(TriggerObject&&) noexcept = default;
	};
	/// <summary>トリガーオブジェクトのリスト。</summary>
	std::vector<std::unique_ptr<TriggerObject>> triggerObjects_;

	/// <summary>トリガー位置の弧長(s)。</summary>
	std::vector<float> triggerS_;
	/// <summary>トリガー発火済みフラグ。</summary>
	std::vector<bool>  triggerFired_;
	/// <summary>前フレームのカメラ位置(弧長)。</summary>
	float prevEyeS_ = 0.0f;
	
	/// <summary>制御点からトリガー位置(s)を再構築する。</summary>
	void RebuildTriggerSFromSegments();


	/// <summary>弧長マップ（距離/パラメータ変換テーブル）。</summary>
	PolylineArc arcMap_;
	/// <summary>現在のカメラ視点位置(s)。</summary>
	float eyeS_ = 0.0f;
	/// <summary>現在のカメラ注視点位置(s)。</summary>
	float forwardS_ = 0.0f;
	/// <summary>注視点への先読み距離。</summary>
	float lookAhead_ = 1.0f;
	/// <summary>移動速度（m/s）。</summary>
	float speedMps_ = 5.0f;
};

