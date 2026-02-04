#include "RailManager.h"
#include "RailEditor.h"
#include "Camera.h"
#include "TImer.h"
#include <imgui.h>

void RailManager::Init()
{
	// JSONからレールデータをロード
	RailEditor::Instance()->Load("Resources/JSON/RailEditor.json");
	
	// ロードしたデータで初期化・再構築
	Reset();
	ResetRailCamera();

	railFinished_ = false;
	railFinishedJustNow_ = false;

#ifdef _DEBUG
	isRailCameraMove_ = false;
#else
	isRailCameraMove_ = true;
#endif // _DEBUG
}

void RailManager::Reset()
{
	controlPoints_ = RailEditor::Instance()->GetControlPoints();
	triggeredFlags_ = std::vector<bool>(controlPoints_.size(), false);
	triggerObjects_.clear();

	const auto& segments = RailEditor::Instance()->GetSegments();
	for (size_t i = 0; i < controlPoints_.size(); ++i)
	{
		if (i < segments.size() && segments[i].triggerEvent)
		{
			triggerObjects_.emplace_back(std::make_unique<TriggerObject>(controlPoints_[i]));
			triggerObjects_.back()->world.Update();
		}
	}

	SetSegment();
	RailReDraw();

	// 距離トリガーを再構築
	RebuildTriggerSFromSegments();

	speedMultiply_ = 0.35f;
	speedMps_ = 5.0f;

	// 弧長の初期値
	eyeS_ = 0.0f;
	forwardS_ = std::min(arcMap_.total, lookAhead_);
	prevEyeS_ = eyeS_;
}

void RailManager::Update()
{
	deltaTime_ = Timer::GetInstance()->GetDeltaTime();
#ifdef _DEBUG
	RailCameraDebug();
#else
	RailCameraMove();
#endif // _DEBUG

}

void RailManager::Draw()
{
#ifdef _DEBUG
	for (const auto& rail : rails_)
	{
		rail->Draw();
	}

	for (auto& triggerObj : triggerObjects_)
	{
		triggerObj->object.Draw(triggerObj->world);
	}
#endif // _DEBUG
}

void RailManager::UpdateEdit()
{
	deltaTime_ = Timer::GetInstance()->GetDeltaTime();
	RailCameraDebug();
	StageEdit();
}


void RailManager::PopRail(const Vector3& position, const Vector3& rotate)
{
	auto rail = std::make_unique<Rail>();
	rail->Init();
	rail->SetTranslation(position);
	rail->SetRotate(rotate);
	rail->Update();
	rails_.push_back(std::move(rail));
}

void RailManager::StageEdit()
{
#ifdef _DEBUG
	RailEditor::Instance()->DrawEditorUI();
	if (RailEditor::Instance()->NeedsPreviewUpdate())
	{
		controlPoints_ = RailEditor::Instance()->GetControlPoints();
		triggeredFlags_ = std::vector<bool>(controlPoints_.size(), false);
		triggerObjects_.clear();

		const auto& segments = RailEditor::Instance()->GetSegments();
		for (size_t i = 0; i < controlPoints_.size(); ++i)
		{
			if (i < segments.size() && segments[i].triggerEvent)
			{
				triggerObjects_.emplace_back(std::make_unique<TriggerObject>(controlPoints_[i]));
				triggerObjects_.back()->world.Update();
			}
		}

		SetSegment();
		RailReDraw();
		RailEditor::Instance()->ResetPreviewFlag();
	}
#endif
}

void RailManager::RailReDraw()
{
	// 制御点からCatmull-Romスプライン曲線上の点を生成
	pointsDrawing_.clear();
	for (size_t i = 0; i < segmentCount + 1; ++i)
	{
		float t = 1.0f / segmentCount * i;
		Vector3 pos = CatmullRomPosition(controlPoints_, t);
		pointsDrawing_.push_back(pos);
	}

	// 描画用のRailオブジェクトを生成・配置
	rails_.clear();
	size_t i = 0;
	for (Vector3& v : pointsDrawing_)
	{
		if (pointsDrawing_[i] == pointsDrawing_.back()) break;
		++i;
		// 次の点への向きに合わせて回転を設定
		Vector3 forward = pointsDrawing_[i] - v;
		Vector3 rotate{};
		rotate.y = std::atan2(forward.x, forward.z);
		float len = Length({ forward.x, 0, forward.z });
		rotate.x = std::atan2(-forward.y, len);
		PopRail(v, rotate);
	}
	
	// 弧長（Arc Length）マップの構築（等速移動のため）
	arcMap_ = BuildPolylineArc(pointsDrawing_);
	eyeS_ = 0.0f;
	forwardS_ = std::min(arcMap_.total, lookAhead_);
}

void RailManager::RailCameraMove()
{
	if (!isRailCameraMove_) return;

	// “今フレーム到達”は毎フレーム先にクリア
	railFinishedJustNow_ = false;

	if (controlPoints_.size() < 4 || pointsDrawing_.size() < 2 || arcMap_.total <= 0.0f)
	{
		isRailCameraMove_ = false;
		return;
	}

	// 前フレーム弧長を保持
	prevEyeS_ = eyeS_;

	// 時間更新（現在の速度設定に応じて進める距離を加算）
	const float ds = speedMps_ * speedMultiply_ * deltaTime_;
	eyeS_ = std::min(eyeS_ + ds, arcMap_.total);
	forwardS_ = std::min(eyeS_ + lookAhead_, arcMap_.total);

	// 距離→t（スプライン全体でのt値）へ変換
	const float eyeT = DistanceToT_Hybrid(arcMap_, eyeS_);
	const float forwardT = DistanceToT_Hybrid(arcMap_, forwardS_);

	// 位置・向きの計算
	Vector3 eye = CatmullRomPosition(controlPoints_, eyeT);
	Vector3 target = CatmullRomPosition(controlPoints_, forwardT);
	Vector3 forward = target - eye;

	// 進行方向ベクトルからオイラー角を算出 (Y-up)
	Vector3 rot{};
	rot.y = std::atan2(forward.x, forward.z);
	const float lenXZ = Length({ forward.x, 0.0f, forward.z });
	rot.x = std::atan2(-forward.y, lenXZ);

	// カメラオフセットを回転させて現在位置に加算
	Matrix4x4 rotMat = MakeRotateXYZMatrix(rot);
	Vector3   upOffset = TransformNormal(offsetCameraPos_, rotMat);
	eye += upOffset;

	// カメラへの反映
	camera_->SetRotate(rot);
	camera_->SetTranslate(eye);

	// ── 終端到達判定 ─────────────────────────────
	// 前フレームは未到達で、今フレームで total に到達/超過したら到達扱い
	constexpr float eps = 1e-6f;
	const bool reachedEnd =
		(prevEyeS_ < arcMap_.total - eps) &&
		((eyeS_ >= arcMap_.total - eps) || (forwardS_ >= arcMap_.total - eps));

	if (reachedEnd)
	{
		railFinished_ = true;
		railFinishedJustNow_ = true;   // このフレームだけ true
		isRailCameraMove_ = false;  // 自動で停止
		return;
	}
}


bool RailManager::RailTrigger()
{
	if (triggerS_.empty()) return false;

	// 通過方向に対応（通常は prevEyeS_ <= eyeS_）
	const float s0 = std::min<float>(prevEyeS_, eyeS_);
	const float s1 = std::max<float>(prevEyeS_, eyeS_);

	bool firedAny = false;

	// 昇順になっているので前から見るだけでOK
	// 登録されたトリガー位置（距離s）が今回の移動区間内にあるかチェック
	for (size_t i = 0; i < triggerS_.size(); ++i)
	{
		if (triggerFired_[i]) continue;

		const float s = triggerS_[i];
		if (s > s1) break;                 // これより先はまだ到達していない
		if (s >= s0 && s <= s1)            // 区間内に入ったらイベント発火
		{
			triggerFired_[i] = true;
			firedAny = true;
			// 複数個を同一フレームで通過しても全部拾える
		}
	}
	return firedAny;
}

void RailManager::RailCameraDebug()
{
#ifdef _DEBUG
	ImGui::Begin("RailCamera");
	if (ImGui::Button("Start/Stop"))
	{
		isRailCameraMove_ = !isRailCameraMove_;
	}
	if (ImGui::Button("ResetCamera"))
	{
		ResetRailCamera();
		RailCameraMove();
	}
	if (isRailCameraMove_)
	{
		RailCameraMove();
	}
	ImGui::DragFloat("SpeedMultiply", &speedMultiply_, 0.1f);
	ImGui::End();
#endif
}

void RailManager::SetSegment()
{
	segmentCount = oneSegmentCount * controlPoints_.size();
}

void RailManager::ResetRailCamera()
{
	// 距離を初期化
	eyeS_ = 0.0f;
	forwardS_ = std::min(lookAhead_, arcMap_.total);

	// 距離から t を計算
	float eyeT = DistanceToT_Hybrid(arcMap_, eyeS_);
	float forwardT = DistanceToT_Hybrid(arcMap_, forwardS_);

	// Catmull-Rom曲線上の座標を取得
	Vector3 eye = CatmullRomPosition(controlPoints_, eyeT);
	Vector3 target = CatmullRomPosition(controlPoints_, forwardT);
	Vector3 forward = target - eye;

	// 回転角の計算
	Vector3 rot{};
	rot.y = std::atan2(forward.x, forward.z);
	float lenXZ = Length({ forward.x, 0.0f, forward.z });
	rot.x = std::atan2(-forward.y, lenXZ);

	// カメラオフセットの適用（回転行列を使用）
	Matrix4x4 rotMat = MakeRotateXYZMatrix(rot);
	Vector3 upOffset = TransformNormal(offsetCameraPos_, rotMat);
	eye += upOffset;

	// カメラへ反映
	if (camera_)
	{
		camera_->SetRotate(rot);
		camera_->SetTranslate(eye);
	}

	// 前フレームの距離リセット
	prevEyeS_ = eyeS_;
}

void RailManager::RebuildTriggerSFromSegments()
{
	triggerS_.clear();
	triggerFired_.clear();

	if (controlPoints_.size() < 2 || pointsDrawing_.size() < 2 || arcMap_.S.empty())
		return;

	const auto& segments = RailEditor::Instance()->GetSegments();

	const size_t Nctrl = controlPoints_.size();
	const size_t Npoly = pointsDrawing_.size();

	// 制御点に対応する弧長（距離）を計算しトリガーリストに追加
	for (size_t i = 0; i < Nctrl && i < segments.size(); ++i)
	{
		if (!segments[i].triggerEvent) continue;

		// 制御点のインデックスからスプライン全体のtを算出（近似）
		float t = (Nctrl <= 1) ? 0.0f : static_cast<float>(i) / static_cast<float>(Nctrl - 1);
		size_t idx = static_cast<size_t>(std::round(t * static_cast<float>(Npoly - 1)));
		idx = std::min(idx, arcMap_.S.size() - 1);

		triggerS_.push_back(arcMap_.S[idx]);
		triggerFired_.push_back(false);
	}

	// s の昇順に整列（安全のため）
	std::vector<size_t> order(triggerS_.size());
	std::iota(order.begin(), order.end(), 0);
	std::sort(order.begin(), order.end(), [&](size_t a, size_t b) { return triggerS_[a] < triggerS_[b]; });

	std::vector<float> sSorted;
	std::vector<bool>  fSorted;
	sSorted.reserve(triggerS_.size());
	fSorted.reserve(triggerS_.size());
	for (size_t k : order) { sSorted.push_back(triggerS_[k]); fSorted.push_back(triggerFired_[k]); }
	triggerS_.swap(sSorted);
	triggerFired_.swap(fSorted);
}
