#include "GameScene.h"
#include "TextureManager.h"
#include <cassert>
#include "mathFunc.h"
#include "operatorOverload.h"
#include "ModelManager.h"
#include "SpriteBasis.h"
#include "Object3dBasis.h"
#include "ParticleClass.h"
#include <fstream>
#include <istream>
#include "../engine/Audio/Audio.h"

#ifdef _DEBUG
#include "imgui.h"
#endif

GameScene::~GameScene()
{
	Audio::GetInstance()->StopStreaming();
}

void GameScene::Init()
{
	input_ = Input::GetInstance();
	camera_ = Object3dBasis::GetInstance()->GetDefaultCamera();
	worldTransform_.Initialize();
	ModelManager::GetInstance()->LoadModel("Resources", "cube.obj");
	ModelManager::GetInstance()->LoadModel("Resources", "skydome.obj");

	Audio::GetInstance()->LoadWave("fanfare");

	obj_ = std::make_unique<Object3d>();
	obj_->Initialize();
	obj_->SetModel("cube.obj");

	skydome_ = std::make_unique<Skydome>();
	skydome_->Initialize();

	RailEditor::Instance()->Load("Resources/JSON/RailEditor.json");
	controlPoints_ = RailEditor::Instance()->GetControlPoints();
	triggeredFlags_ = std::vector<bool>(controlPoints_.size(), false);
	triggerObjects_.clear();

	const auto& segments = RailEditor::Instance()->GetSegments();
	for (size_t i = 0; i < controlPoints_.size(); ++i)
	{
		if (i < segments.size() && segments[i].triggerEvent)
		{
			triggerObjects_.emplace_back(std::make_unique<TriggerObject>(controlPoints_[i]));
		}
	}

	segmentCount = oneSegmentCount * controlPoints_.size();
	SetSegment();
	RailLineReDraw();
	RailReDraw();
	ResetRailCamera();
}

void GameScene::Update()
{
	RailCameraDebug();
	RailCustom();
	skydome_->Update();

	for (auto& trigger : triggerObjects_)
	{
		trigger->world.TransferMatrix();
	}
}

void GameScene::Draw()
{
	Object3dBasis::GetInstance()->BasisDrawSetting();
	skydome_->Draw();

	for (const auto& rail : rails_)
	{
		rail->Draw();
	}
	for (auto& trigger : triggerObjects_)
	{
		trigger->object.Draw(trigger->world);
	}

	SpriteBasis::GetInstance()->BasisDrawSetting();
}

void GameScene::CheckAllCollisions()
{
}

void GameScene::PopRail(Vector3 position, Vector3 rota)
{
	auto rail = std::make_unique<Rail>();
	rail->Initialize(position);
	rail->SetRotate(rota);
	rail->UpdateTransform();
	rails_.push_back(std::move(rail));
}

void GameScene::RailCustom()
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
			}
		}

		segmentCount = oneSegmentCount * controlPoints_.size();
		SetSegment();
		RailLineReDraw();
		RailReDraw();
		RailEditor::Instance()->ResetPreviewFlag();
	}
#endif
}

void GameScene::RailLineReDraw()
{
	pointsDrawing_.clear();
	for (size_t i = 0; i < segmentCount + 1; ++i)
	{
		float t = 1.0f / segmentCount * i;
		Vector3 pos = CatmullRomPosition(controlPoints_, t);
		pointsDrawing_.push_back(pos);
	}
	rails_.clear();
	for (auto& pos : controlPoints_)
	{
		PopRail(pos, { 0, 0, 0 });
	}
}

void GameScene::RailReDraw()
{
	rails_.clear();
	size_t i = 0;
	for (Vector3& v : pointsDrawing_)
	{
		if (pointsDrawing_[i] == pointsDrawing_.back()) break;
		++i;
		Vector3 forward = pointsDrawing_[i] - v;
		Vector3 rotate{};
		rotate.y = std::atan2(forward.x, forward.z);
		float len = Length({ forward.x, 0, forward.z });
		rotate.x = std::atan2(-forward.y, len);
		PopRail(v, rotate);
	}
}

void GameScene::RailCameraMove()
{
	if (cameraForwardT <= 1.0f)
	{
		cameraEyeT += cameraSegmentCount;
		cameraForwardT += cameraSegmentCount;
		Vector3 eye = CatmullRomPosition(controlPoints_, cameraEyeT);
		eye.y += 0.5f;
		camera_->SetTranslate(eye);

		Vector3 forward = CatmullRomPosition(controlPoints_, cameraForwardT);
		forward.y += 0.5f;
		forward = forward - eye;

		if (cameraForwardT <= 1.0f)
		{
			Vector3 rot{};
			rot.y = std::atan2(forward.x, forward.z);
			float len = Length({ forward.x, 0, forward.z });
			rot.x = std::atan2(-forward.y, len);
			camera_->SetRotate(rot);
		}

		size_t currentIndex = static_cast<size_t>(cameraEyeT * controlPoints_.size());
		const auto& segments = RailEditor::Instance()->GetSegments();
		if (currentIndex < segments.size() && segments[currentIndex].triggerEvent)
		{
			if (alreadyTriggeredIndices_.find(currentIndex) == alreadyTriggeredIndices_.end())
			{
				Audio::GetInstance()->PlayWave("fanfare");
				alreadyTriggeredIndices_.insert(currentIndex);
			}
		}
	}
	else
	{
		isRailCameraMove_ = false;
	}

#ifdef _DEBUG
	ImGui::Begin("RailCamera");
	ImGui::Text("eye%.03f", cameraEyeT);
	ImGui::Text("forward%.03f", cameraForwardT);
	ImGui::End();
#endif
}

void GameScene::RailCameraDebug()
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
	ImGui::End();
#endif
}

void GameScene::SetSegment()
{
	float denom = kDivisionSpan * controlPoints_.size();
	cameraSegmentCount = 1.0f / denom;
}

void GameScene::ResetRailCamera()
{
	float denom = kDivisionSpan * controlPoints_.size();
	cameraEyeT = 0;
	cameraForwardT = 30.0f / denom;
}