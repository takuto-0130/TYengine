#include "RailManager.h"
#include "RailEditor.h"
#include "Camera.h"
#include "TextureManager.h"
#include "TImer.h"
#include <imgui.h>

using namespace TYEngine::Utility;

void RailManager::Init()
{
	//// JSONからレールデータをロード
	//RailEditor::Instance()->Load("Resources/JSON/RailEditor.json");
	//
	//// ロードしたデータで初期化・再構築
	//Reset();
	//ResetRailCamera();

	TYEngine::Graphics::TextureManager::GetInstance()->LoadTexture("Resources/Texture/white2x2.png");

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
	// 動的モードでなければ、エディタからデータを取得する
	if (!isDynamicMode_)
	{
		controlPoints_ = RailEditor::Instance()->GetControlPoints();
	}

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
	speedMps_ = 3.5f;

	// 弧長の初期値
	eyeS_ = 0.0f;
	forwardS_ = std::min(arcMap_.total, lookAhead_);
	prevEyeS_ = eyeS_;
}

void RailManager::SetDynamicData(const std::vector<TYEngine::Utility::Vector3>& points, const std::vector<bool>& triggers)
{
	isDynamicMode_ = true;

	// 1. 制御点を直接上書き
	controlPoints_ = points;

	// 2. トリガーフラグとオブジェクトの初期化
	triggeredFlags_ = std::vector<bool>(controlPoints_.size(), false);
	triggerObjects_.clear();

	// 3. 受け取ったトリガー情報をもとにオブジェクトを配置
	for (size_t i = 0; i < controlPoints_.size(); ++i)
	{
		// triggers配列の範囲外アクセスを防ぎつつ取得
		bool isTrigger = (i < triggers.size()) ? triggers[i] : false;

		if (isTrigger)
		{
			triggerObjects_.emplace_back(std::make_unique<TriggerObject>(controlPoints_[i]));
			triggerObjects_.back()->world.Update();
		}
	}

	// 4. スプライン路線の再計算と描画用オブジェクトの構築
	SetSegment();
	RailReDraw();

	// デバッグ出力: レールの描画用ポイントをCSVに書き出す
	std::ofstream ofs("RailDebug.csv");
	ofs << "X,Y,Z\n";
	for (const auto& p : pointsDrawing_)
	{
		ofs << p.x << "," << p.y << "," << p.z << "\n";
	}
	ofs.close();

	// 5. 進行状況やカメラをスタート地点にリセット
	ResetRailCamera();

	railFinished_ = false;
	railFinishedJustNow_ = false;
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
	int num = 0;
	for (const auto& rail : rails_)
	{
		if(num % 5 == 0)
		{
			rail->Draw();
		}
		num++;
	}

	for (auto& triggerObj : triggerObjects_)
	{
		triggerObj->object.Draw(triggerObj->world);
	}
#endif // _DEBUG

	for (auto& envObj : environmentObjects_)
	{
		envObj->object.Draw(envObj->world);
	}

	// 地形メッシュの描画
	if (terrainObject_)
	{
		terrainObject_->Draw(terrainTransform_);
	}
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

void RailManager::GenerateForest()
{
	environmentObjects_.clear();
	if (pointsDrawing_.size() < 2) return;

	srand(12345);
	const int SPAWN_INTERVAL = 5;

	// =======================================================
	// レールの進行度(s)を計算
	// =======================================================
	std::vector<float> sValues(pointsDrawing_.size(), 0.0f);
	for (size_t i = 1; i < pointsDrawing_.size(); ++i)
	{
		float dx = pointsDrawing_[i].x - pointsDrawing_[i - 1].x;
		float dy = pointsDrawing_[i].y - pointsDrawing_[i - 1].y;
		float dz = pointsDrawing_[i].z - pointsDrawing_[i - 1].z;
		sValues[i] = sValues[i - 1] + std::sqrt(dx * dx + dy * dy + dz * dz);
	}

	// =======================================================
	// フラクタルノイズによる複雑な地形の起伏
	// =======================================================
	auto getTerrainHeight = [](float s, float lateral)
		{
			float absLat = std::abs(lateral);
			float roadWidth = 8.0f; // 平らな道の半分の幅

			if (absLat < roadWidth) return 0.0f;

			// 大きなうねり
			float noiseLarge = std::sin(s * 0.03f + lateral * 0.05f) * 6.0f
				+ std::cos(s * 0.05f - lateral * 0.03f) * 4.0f;

			// 小さなうねり
			float noiseSmall = std::sin(s * 0.2f + lateral * 0.15f) * 1.5f
				+ std::cos(s * 0.3f - lateral * 0.25f) * 0.8f;

			float baseHeight = std::pow(absLat - roadWidth, 1.2f) * 0.25f; // U字谷のベース
			float blend = std::min((absLat - roadWidth) / 5.0f, 1.0f);

			return baseHeight + ((noiseLarge + noiseSmall) * blend);
		};

	// 乱数生成ヘルパー (min ～ max の間の小数を返す)
	auto randomFloat = [](float min, float max)
		{
			return min + static_cast<float>(rand()) / (static_cast<float>(RAND_MAX) / (max - min));
		};

	// =======================================================
	// グリッドの構築（ワールドX軸固定 ＆ 動的ジッター）
	// =======================================================
	const int LATERAL_DIVISIONS = 10;
	const float MAX_LATERAL_DIST = 60.0f;

	// 横方向の1マスの道幅
	const float COL_DIST = MAX_LATERAL_DIST / static_cast<float>(LATERAL_DIVISIONS);

	std::vector<std::vector<TYEngine::Utility::Vector3>> crossSections(pointsDrawing_.size());

	for (size_t i = 0; i < pointsDrawing_.size(); ++i)
	{
		TYEngine::Utility::Vector3 currentPos = pointsDrawing_[i];

		TYEngine::Utility::Vector3 right = { 1.0f, 0.0f, 0.0f };

		// 前後の頂点間隔（行の間隔）を動的に取得する
		float rowDist = 1.0f;
		if (i < pointsDrawing_.size() - 1)
		{
			rowDist = pointsDrawing_[i + 1].z - pointsDrawing_[i].z;
		}
		else if (i > 0)
		{
			rowDist = pointsDrawing_[i].z - pointsDrawing_[i - 1].z;
		}
		if (rowDist < 0.001f) rowDist = 1.0f;

		// マスの大きさを超えないように、安全なジッターの最大量を「マスの40%まで」に制限する
		float maxJitterX = COL_DIST * 0.4f;
		float maxJitterZ = rowDist * 0.4f;

		for (int j = -LATERAL_DIVISIONS; j <= LATERAL_DIVISIONS; ++j)
		{
			float t = static_cast<float>(j) / static_cast<float>(LATERAL_DIVISIONS);
			float baseLateralDist = t * MAX_LATERAL_DIST;

			// 道の中央（jが0付近）はレールに沿わせるためズラさない
			float jitterAmountX = (std::abs(j) <= 1) ? 0.0f : maxJitterX;
			float jitterAmountZ = (std::abs(j) <= 1) ? 0.0f : maxJitterZ;

			// 端や境界は隙間防止のためズラさない
			if (i == 0 || i == pointsDrawing_.size() - 1 || std::abs(j) == LATERAL_DIVISIONS)
			{
				jitterAmountX = 0.0f;
				jitterAmountZ = 0.0f;
			}

			float jitterX = randomFloat(-jitterAmountX, jitterAmountX); // 横方向のズレ
			float jitterZ = randomFloat(-jitterAmountZ, jitterAmountZ); // 進行方向のズレ

			float finalLateralDist = baseLateralDist + jitterX;
			float height = getTerrainHeight(sValues[i] + jitterZ, finalLateralDist);

			// 世界軸（XとZ）に対して真っ直ぐグリッドを配置（中心のみレールの座標に追従）
			TYEngine::Utility::Vector3 pos = {
				currentPos.x + finalLateralDist,
				currentPos.y - 2.0f + height,
				currentPos.z + jitterZ
			};
			crossSections[i].push_back(pos);
		}
	}

	// =======================================================
	// オブジェクトの配置
	// =======================================================
	for (size_t i = 0; i < pointsDrawing_.size() - 1; i += SPAWN_INTERVAL)
	{
		TYEngine::Utility::Vector3 currentPos = pointsDrawing_[i];

		TYEngine::Utility::Vector3 right = { 1.0f, 0.0f, 0.0f };

		for (int side : {-1, 1})
		{
			bool isTree = (rand() % 100) < 70;
			float baseDist = isTree ? 18.0f : 12.0f;
			float randomOffset = static_cast<float>(rand() % 250) / 10.0f;
			float lateralDist = (baseDist + randomOffset) * side;

			if (std::abs(lateralDist) > MAX_LATERAL_DIST - 5.0f) lateralDist = (MAX_LATERAL_DIST - 5.0f) * side;

			float height = getTerrainHeight(sValues[i], lateralDist);

			TYEngine::Utility::Vector3 spawnPos = {
				currentPos.x + right.x * lateralDist,
				currentPos.y - 2.5f + height,
				currentPos.z + right.z * lateralDist
			};

			auto envObj = std::make_unique<EnvironmentObject>();
			envObj->type = isTree ? 0 : 1;
			envObj->world.Initialize();
			envObj->world.SetTranslation(spawnPos);

			float randomRotY = randomFloat(0.0f, 360.0f) * 3.14159f / 180.0f;
			envObj->world.SetRotate({ 0.0f, randomRotY, 0.0f });

			float randomScale = 0.8f + randomFloat(0.0f, 0.7f);
			if (isTree) envObj->world.SetScale({ randomScale, randomScale * 1.2f, randomScale });
			else        envObj->world.SetScale({ randomScale, randomScale * 0.5f, randomScale });

			envObj->object.Initialize();
			envObj->object.SetModel("conifer.obj");
			if (!isTree) envObj->object.SetColor({ 0.1f, 0.1f, 0.1f, 1.0f });

			envObj->world.Update();
			environmentObjects_.push_back(std::move(envObj));
		}
	}

	// =======================================================
	// ポリゴンメッシュの構築
	// =======================================================
	std::vector<TYEngine::Graphics::Model::VertexData> vertices;
	float currentV = 0.0f;

	auto calcNormal = [](const TYEngine::Utility::Vector3& pA, const TYEngine::Utility::Vector3& pB, const TYEngine::Utility::Vector3& pC)
		{
			TYEngine::Utility::Vector3 v1 = { pB.x - pA.x, pB.y - pA.y, pB.z - pA.z };
			TYEngine::Utility::Vector3 v2 = { pC.x - pA.x, pC.y - pA.y, pC.z - pA.z };
			TYEngine::Utility::Vector3 cross = { v1.y * v2.z - v1.z * v2.y, v1.z * v2.x - v1.x * v2.z, v1.x * v2.y - v1.y * v2.x };
			float len = std::sqrt(cross.x * cross.x + cross.y * cross.y + cross.z * cross.z);
			if (len > 0.0001f) { cross.x /= len; cross.y /= len; cross.z /= len; }
			return cross;
		};

	auto addQuad = [&](const TYEngine::Utility::Vector3& pA, const TYEngine::Utility::Vector3& pB, const TYEngine::Utility::Vector3& pC, const TYEngine::Utility::Vector3& pD, float uStart, float uEnd, float vStart, float vEnd)
		{
			TYEngine::Utility::Vector3 normal1 = calcNormal(pA, pB, pC);
			TYEngine::Utility::Vector3 normal2 = calcNormal(pA, pC, pD);
			vertices.push_back({ {pA.x, pA.y, pA.z, 1.0f}, {uStart, vStart}, normal1 });
			vertices.push_back({ {pB.x, pB.y, pB.z, 1.0f}, {uStart, vEnd},   normal1 });
			vertices.push_back({ {pC.x, pC.y, pC.z, 1.0f}, {uEnd,   vEnd},   normal1 });
			vertices.push_back({ {pA.x, pA.y, pA.z, 1.0f}, {uStart, vStart}, normal2 });
			vertices.push_back({ {pC.x, pC.y, pC.z, 1.0f}, {uEnd,   vEnd},   normal2 });
			vertices.push_back({ {pD.x, pD.y, pD.z, 1.0f}, {uEnd,   vStart}, normal2 });
		};

	for (size_t i = 0; i < crossSections.size() - 1; ++i)
	{
		float dist = sValues[i + 1] - sValues[i];
		float nextV = currentV + (dist * 0.1f);

		int numPoints = LATERAL_DIVISIONS * 2 + 1;
		for (int j = 0; j < numPoints - 1; ++j)
		{
			float uStart = static_cast<float>(j) / static_cast<float>(numPoints - 1);
			float uEnd = static_cast<float>(j + 1) / static_cast<float>(numPoints - 1);

			addQuad(
				crossSections[i][j], crossSections[i + 1][j],
				crossSections[i + 1][j + 1], crossSections[i][j + 1],
				uStart, uEnd, currentV, nextV
			);
		}
		currentV = nextV;
	}

	if (!vertices.empty())
	{
		terrainModel_.reset();
		terrainModel_ = std::make_unique<TYEngine::Graphics::Model>();
		terrainModel_->InitializeDynamic(TYEngine::Graphics::ModelManager::GetInstance()->GetModelLoader(), vertices, "Resources/Models/grassfloor1.dds");
		terrainObject_ = std::make_unique<TYEngine::Graphics::Object3d>();
		terrainObject_->Initialize();
		terrainObject_->SetModel(terrainModel_.get());
		terrainTransform_.Initialize();
	}
}

//void RailManager::GenerateTerrainMesh()
//{
//	if (pointsDrawing_.size() < 2) return;
//
//	std::vector<TYEngine::Graphics::Model::VertexData> vertices;
//
//	float roadWidth = 20.0f;
//	float slopeWidth = 40.0f;
//	float slopeHeight = 25.0f;
//
//	// UVのV座標（縦方向の進行度）を計算するための距離アキュムレータ
//	float currentV = 0.0f;
//
//	// レールの各ポイントを繋ぐ面（Quad = 2個の三角形 = 6頂点）を作っていく
//	for (size_t i = 0; i < pointsDrawing_.size() - 1; ++i)
//	{
//		TYEngine::Utility::Vector3 p0 = pointsDrawing_[i];
//		TYEngine::Utility::Vector3 p1 = pointsDrawing_[i + 1];
//
//		// --- ベクトルの計算 ---
//		auto calcVectors = [](const TYEngine::Utility::Vector3& current, const TYEngine::Utility::Vector3& next,
//			TYEngine::Utility::Vector3& outForward, TYEngine::Utility::Vector3& outRight, float& outDist)
//			{
//				outForward = { next.x - current.x, next.y - current.y, next.z - current.z };
//				outDist = std::sqrt(outForward.x * outForward.x + outForward.y * outForward.y + outForward.z * outForward.z);
//				if (outDist > 0.0001f) { outForward.x /= outDist; outForward.y /= outDist; outForward.z /= outDist; }
//
//				TYEngine::Utility::Vector3 up = { 0.0f, 1.0f, 0.0f };
//				outRight = { up.y * outForward.z - up.z * outForward.y,
//							 up.z * outForward.x - up.x * outForward.z,
//							 up.x * outForward.y - up.y * outForward.x };
//				float rLen = std::sqrt(outRight.x * outRight.x + outRight.y * outRight.y + outRight.z * outRight.z);
//				if (rLen > 0.0001f) { outRight.x /= rLen; outRight.y /= rLen; outRight.z /= rLen; }
//			};
//
//		TYEngine::Utility::Vector3 forward0, right0; float dist0;
//		calcVectors(p0, p1, forward0, right0, dist0);
//
//		// ※本来は次の点でのベクトルも計算して滑らかに繋ぎますが、ここでは簡略化のため同じベクトルを使います
//		TYEngine::Utility::Vector3 forward1 = forward0, right1 = right0;
//
//		// --- 頂点座標の計算関数 ---
//		auto getCrossSection = [&](const TYEngine::Utility::Vector3& center, const TYEngine::Utility::Vector3& right)
//			{
//				TYEngine::Utility::Vector3 base = { center.x, center.y - 2.0f, center.z };
//
//				// L2(左崖上), L1(左道端), R1(右道端), R2(右崖上)
//				return std::vector<TYEngine::Utility::Vector3>{
//					{ base.x - right.x * (roadWidth / 2 + slopeWidth), base.y + slopeHeight, base.z - right.z * (roadWidth / 2 + slopeWidth) },
//					{ base.x - right.x * (roadWidth / 2),              base.y,               base.z - right.z * (roadWidth / 2) },
//					{ base.x + right.x * (roadWidth / 2),              base.y,               base.z + right.z * (roadWidth / 2) },
//					{ base.x + right.x * (roadWidth / 2 + slopeWidth), base.y + slopeHeight, base.z + right.z * (roadWidth / 2 + slopeWidth) }
//				};
//			};
//
//		auto cross0 = getCrossSection(p0, right0);
//		auto cross1 = getCrossSection(p1, right1);
//
//		// UV座標のV値 (前進するごとにテクスチャがスクロールする)
//		float nextV = currentV + (dist0 * 0.1f); // 0.1fはテクスチャの繰り返し密度
//
//		// --- ポリゴン（三角形リスト）の構築 ---
//		// 四角形(Quad)を作るためのヘルパー関数 (pA, pB, pC, pD は反時計回りになるように渡す)
//		auto addQuad = [&](
//			const TYEngine::Utility::Vector3& pA, const TYEngine::Utility::Vector3& pB,
//			const TYEngine::Utility::Vector3& pC, const TYEngine::Utility::Vector3& pD,
//			float uStart, float uEnd, float vStart, float vEnd, const TYEngine::Utility::Vector3& normal)
//			{
//				// 三角形1 (A -> B -> C)
//				vertices.push_back({ {pA.x, pA.y, pA.z, 1.0f}, {uStart, vStart}, normal });
//				vertices.push_back({ {pB.x, pB.y, pB.z, 1.0f}, {uStart, vEnd},   normal });
//				vertices.push_back({ {pC.x, pC.y, pC.z, 1.0f}, {uEnd,   vEnd},   normal });
//				// 三角形2 (A -> C -> D)
//				vertices.push_back({ {pA.x, pA.y, pA.z, 1.0f}, {uStart, vStart}, normal });
//				vertices.push_back({ {pC.x, pC.y, pC.z, 1.0f}, {uEnd,   vEnd},   normal });
//				vertices.push_back({ {pD.x, pD.y, pD.z, 1.0f}, {uEnd,   vStart}, normal });
//			};
//
//		// 1. 左の斜面
//		addQuad(cross0[0], cross1[0], cross1[1], cross0[1], 0.0f, 0.2f, currentV, nextV, { 0.8f, 0.6f, 0.0f }); // 法線は仮
//		// 2. 中央の道
//		addQuad(cross0[1], cross1[1], cross1[2], cross0[2], 0.2f, 0.8f, currentV, nextV, { 0.0f, 1.0f, 0.0f }); // 上向き
//		// 3. 右の斜面
//		addQuad(cross0[2], cross1[2], cross1[3], cross0[3], 0.8f, 1.0f, currentV, nextV, { -0.8f, 0.6f, 0.0f });
//
//		currentV = nextV;
//	}
//
//	// 生成した頂点データからモデルとObject3dを作成する
//	// ※ ModelLoader は既存の仕組みに合わせて適宜取得してください
//	terrainModel_ = std::make_unique<TYEngine::Graphics::Model>();
//	terrainModel_->InitializeDynamic(TYEngine::Graphics::ModelManager::GetInstance()->GetModelLoader(), vertices, "Resources/Texture/white2x2.png");
//
//	terrainObject_ = std::make_unique<TYEngine::Graphics::Object3d>();
//	terrainObject_->Initialize();
//
//	// Object3dにモデルをセットする仕組みが必要（setterがない場合はObject3dクラスに追加してください）
//	terrainObject_->SetModel(terrainModel_.get());
//
//	// 位置などの初期化
//	terrainTransform_.Initialize();
//	terrainTransform_.UpdateMatrix();
//}
//
//void RailManager::GenerateForest()
//{
//	environmentObjects_.clear();
//
//	// レールの描画用ポイント（滑らかに分割された座標）が十分になければ終了
//	if (pointsDrawing_.size() < 2) return;
//
//	// 曲ごとに同じ森が生成されるように、シード値を固定する(現在はテスト用)
//	srand(12345);
//
//	// 密集しすぎないように、例えば 5ポイント（数メートル）ごとに判定する
//	const int SPAWN_INTERVAL = 5;
//
//	for (size_t i = 0; i < pointsDrawing_.size() - 1; i += SPAWN_INTERVAL)
//	{
//		TYEngine::Utility::Vector3 currentPos = pointsDrawing_[i];
//		TYEngine::Utility::Vector3 nextPos = pointsDrawing_[i + 1];
//
//		// --- 1. 進行方向(Forward)と右方向(Right)のベクトルを計算 ---
//		TYEngine::Utility::Vector3 forward = {
//			nextPos.x - currentPos.x,
//			nextPos.y - currentPos.y,
//			nextPos.z - currentPos.z
//		};
//
//		// 進行方向ベクトルの正規化（長さを1にする）
//		float fLen = std::sqrt(forward.x * forward.x + forward.y * forward.y + forward.z * forward.z);
//		if (fLen > 0.0001f) { forward.x /= fLen; forward.y /= fLen; forward.z /= fLen; }
//
//		TYEngine::Utility::Vector3 up = { 0.0f, 1.0f, 0.0f };
//
//		// 外積(Cross)を使って右方向ベクトルを求める: Right = Up × Forward
//		TYEngine::Utility::Vector3 right = {
//			up.y * forward.z - up.z * forward.y,
//			up.z * forward.x - up.x * forward.z,
//			up.x * forward.y - up.y * forward.x
//		};
//		// 右方向ベクトルの正規化
//		float rLen = std::sqrt(right.x * right.x + right.y * right.y + right.z * right.z);
//		if (rLen > 0.0001f) { right.x /= rLen; right.y /= rLen; right.z /= rLen; }
//
//		// --- 2. 左右への配置（ランダム要素を加える） ---
//
//		// 両脇に配置するため、左(-1)と右(+1)でループ
//		for (int side : {-1, 1})
//		{
//			// 70%の確率で木、30%の確率で岩を生成すると仮定
//			bool isTree = (rand() % 100) < 70;
//
//			// レールからの基本距離（近すぎると自機と被るため少し離す）
//			float baseDistance = isTree ? 15.0f : 8.0f;
//
//			// ランダムな揺らぎ（道幅のバラつき）を加える (0.0f ～ 10.0f)
//			float randomOffset = static_cast<float>(rand() % 100) / 10.0f;
//			float distance = baseDistance + randomOffset;
//
//			// 最終的な配置座標
//			TYEngine::Utility::Vector3 spawnPos = {
//				currentPos.x + (right.x * distance * side),
//				currentPos.y + (right.y * distance * side),
//				currentPos.z + (right.z * distance * side)
//			};
//
//			// 少し高さを下げる（空中に浮かないようにレールより少し下にする）
//			spawnPos.y -= 2.0f;
//
//			// --- 3. オブジェクトの生成と設定 ---
//			auto envObj = std::make_unique<EnvironmentObject>();
//			envObj->type = isTree ? 0 : 1;
//
//			envObj->world.Initialize();
//			envObj->world.SetTranslation(spawnPos);
//
//			// 【重要】自然に見せるためのランダム回転とスケール
//			// Y軸（縦軸）に対してランダムに回転させる
//			float randomRotY = static_cast<float>(rand() % 360) * 3.14159f / 180.0f;
//			envObj->world.SetRotate({ 0.0f, randomRotY, 0.0f });
//
//			// 大きさもバラバラにする (例: 0.8倍 ～ 1.5倍)
//			float randomScale = 0.8f + static_cast<float>(rand() % 70) / 100.0f;
//			if (isTree)
//			{
//				envObj->world.SetScale({ randomScale, randomScale * 1.2f, randomScale }); // 木は縦長に
//			}
//			else
//			{
//				envObj->world.SetScale({ randomScale, randomScale * 0.5f, randomScale }); // 岩は平べったく
//			}
//
//			envObj->object.Initialize();
//
//			// モデルの割り当て（プロジェクト内にある obj ファイル名に変更してください）
//			if (isTree)
//			{
//				envObj->object.SetModel("conifer.obj");
//			}
//			else
//			{
//				envObj->object.SetModel("conifer.obj");
//				envObj->object.SetColor({ 0.1f, 0.1f, 0.1f, 1.0f }); // 灰色
//			}
//
//			// ワールド行列を更新してリストに追加
//			envObj->world.Update();
//			environmentObjects_.push_back(std::move(envObj));
//		}
//	}
//
//	GenerateTerrainMesh();
//}
