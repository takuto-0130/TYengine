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
#endif // DEBUG_

GameScene::~GameScene() {
	Audio::GetInstance()->StopStreaming();
}

void GameScene::Init() {
	input_ = Input::GetInstance();

	camera_ = Object3dBasis::GetInstance()->GetDefaultCamera();

	/*Audio::GetInstance()->SetPitch(pitch_);
	Audio::GetInstance()->StartStreaming("BGM_2.wav", true);*/

	worldTransform_.Initialize();
	ModelManager::GetInstance()->LoadModel("Resources", "cube.obj");
	ModelManager::GetInstance()->LoadModel("Resources", "skydome.obj");

	obj_ = std::make_unique<Object3d>();
	obj_->Initialize();
	obj_->SetModel("cube.obj");
	skydome_ = std::make_unique<Skydome>();
	skydome_->Initialize();

#pragma region // レール設定
	controlPoints_ = {
		{0,  0,  0},
		{10, 10, 10},
		{10, 15, 20},
		{20, 15, 30},
		{20, 0,  35},
		{30, 0,  40},
		{30, 0,  50},
		{40, 0,  55}
	};
	segmentCount = oneSegmentCount * controlPoints_.size();
	for (size_t i = 0; i < segmentCount + 1; i++) {
		float t = 1.0f / segmentCount * i;
		Vector3 pos = CatmullRomPosition(controlPoints_, t);
		pointsDrawing_.push_back(pos);
	}
	size_t i = 0;
	for (Vector3& v : pointsDrawing_) {
		if (pointsDrawing_[i] == pointsDrawing_.back()) {
			break;
		}
		++i;
		Vector3 rotateRail{};
		Vector3 forward = pointsDrawing_[i] - v;
		rotateRail.y = std::atan2(forward.x, forward.z);
		float length = Length({ forward.x, 0, forward.z });
		rotateRail.x = std::atan2(-forward.y, length);
		PopRail(v, rotateRail);
	}
	SetSegment();
	ResetRailCamera();
#pragma endregion
}

#pragma region // 初期化以外
void GameScene::Update() {

	RailCameraDebug();
	RailCustom();

	skydome_->Update();

#ifdef _DEBUG
#endif // _DEBUG


}

void GameScene::Draw() {
#pragma region 背景
	// 背景スプライト描画前
	//SpriteBasis::GetInstance()->BasisDrawSetting();

	// ↓背景

#pragma endregion

#pragma region 3Dオブジェクト
	// 3Dオブジェクト描画前
	Object3dBasis::GetInstance()->BasisDrawSetting();

	skydome_->Draw();

	for (const auto& rail : rails_) 
	{
		rail->Draw();
	}

#pragma endregion


#pragma region 前景
	// 前景スプライト描画前
	SpriteBasis::GetInstance()->BasisDrawSetting();
	/// ↓前景
#pragma endregion
}

void GameScene::CheckAllCollisions()
{
}

void GameScene::RailCustom()
{
#ifdef _DEBUG
	int32_t i = 0;
	ImGui::Begin("Rail");
	ImGui::Text("ReDraw : P");
	for (Vector3& pos : controlPoints_) 
	{
		i++;
		/*ImGui::Text("%d.", i);
		ImGui::SameLine();*/
		std::string label = "controlPoint." + std::to_string(i);
		ImGui::DragFloat3(label.c_str(), &pos.x, 0.1f);
	}
	if (ImGui::Button("addControlPoint"))
	{
		Vector3 pos = controlPoints_.back();
		controlPoints_.push_back(pos);
		segmentCount = oneSegmentCount * controlPoints_.size();
		SetSegment();
		RailLineReDraw();
	}
	ImGui::End();
#endif // _DEBUG
}

void GameScene::RailLineReDraw()
{
	pointsDrawing_.clear();
	for (size_t i = 0; i < segmentCount + 1; i++)
	{
		float t = 1.0f / segmentCount * i;
		Vector3 pos = CatmullRomPosition(controlPoints_, t);
		pointsDrawing_.push_back(pos);
	}
	rails_.clear();/*remove_if([](Rail* rail)
		{
		if (!rail->IsDead()) 
		{
			delete rail;
			return true;
		}
		return false;
		});*/
	for (Vector3& pos : controlPoints_) 
	{
		PopRail(pos, { 0,0,0 });
	}
}

void GameScene::RailReDraw()
{
	rails_.clear();/*remove_if([](Rail* rail)
		{
		if (!rail->IsDead()) 
		{
			delete rail;
			return true;
		}
		return false;
		});*/

	size_t i = 0;
	for (Vector3& v : pointsDrawing_) 
	{
		if (pointsDrawing_[i] == pointsDrawing_.back()) 
		{
			break;
		}
		++i;
		Vector3 rotateRail{};
		Vector3 forward = pointsDrawing_[i] - v;
		// Y軸周り角度(θy)
		rotateRail.y = std::atan2(forward.x, forward.z);
		float length = Length({ forward.x, 0, forward.z });
		// X軸周り角度(θx)
		rotateRail.x = std::atan2(-forward.y, length);
		PopRail(v, rotateRail);
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
			Vector3 rotateCametra{};
			// Y軸周り角度(θy)
			rotateCametra.y = std::atan2(forward.x, forward.z);
			float length = Length({ forward.x, 0, forward.z });
			// X軸周り角度(θx)
			rotateCametra.x = std::atan2(-forward.y, length);
			camera_->SetRotate(rotateCametra);
		}
	}
#ifdef _DEBUG
	ImGui::Begin("RailCamera");
	ImGui::Text("eye%.03f", cameraEyeT);
	ImGui::Text("forward%.03f", cameraForwardT);
	ImGui::End();
#endif // _DEBUG
}

void GameScene::RailCameraDebug()
{
#ifdef _DEBUG
	ImGui::Begin("RailCamera");
	if (ImGui::Button("StartCamera")) 
	{
		isRailCameraMove_ = true;
	}

	if (ImGui::Button("StopCamera"))
	{
		isRailCameraMove_ = false;
	}

	if (ImGui::Button("ResetCamera")) 
	{
		ResetRailCamera();
		RailCameraMove();
	}

	if (isRailCameraMove_) {
		RailCameraMove();
	}
	ImGui::End();
#endif // _DEBUG
}

void GameScene::SetSegment()
{
	float segmentDenominator = kDivisionSpan * controlPoints_.size();
	cameraSegmentCount = 1.0f / segmentDenominator;
}

void GameScene::ResetRailCamera()
{
	float segmentDenominator = kDivisionSpan * controlPoints_.size();
	cameraEyeT = 0;
	cameraForwardT = 30.0f / segmentDenominator;
}
