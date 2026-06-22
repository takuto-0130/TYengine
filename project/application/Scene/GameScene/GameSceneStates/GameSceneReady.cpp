#include "../GameScene.h"
#include "../StartUI/StartUI.h"
#include "Timer.h"
#ifdef _DEBUG
#include "imgui.h"
#endif // _DEBUG

using namespace TYEngine::Utility;

void GameSceneStateReady::Init(GameScene& owner)
{
	owner.startCameraTimer_ = 0;
	owner.prevStateElapsed_ = 0;
}
void GameSceneStateReady::Update(GameScene& owner, float deltaTime)
{
	(void)deltaTime;
	owner.startDraw_->Update();
	owner.startCameraTimer_ += Timer::GetInstance()->GetDeltaTime();
	owner.StartCamera();
}
void GameSceneStateReady::Exit(GameScene& owner)
{
	owner.gameAudio_->Resume(owner.BGMHandle_);
	owner.startDraw_->Reset();
	owner.startCameraTimer_ = 0;
	owner.prevStateElapsed_ = 0;
}

void GameScene::StartCamera()
{
	Vector3 pos = { 0,0,7 };

	float cur = stateMachine_.GetStateElapsedTime();
	float prev = prevStateElapsed_;

	// 演出タイマーのリセット用
	std::function<void(float)> resetTimer = [&](float threshold)
		{

			if (prev < threshold && cur >= threshold)
			{
				startCameraTimer_ = 0;
			}
		};
	prevStateElapsed_ = cur;


	// カメラワークを適用
	std::function<void(Vector3&)> applyCameraWork = [&](Vector3& pos)
		{
			// 真後ろから始まるように調整
			Matrix4x4 rotAjust = MakeRotateYMatrix(0.78f);
			pos = TransformM(pos, rotAjust);

			// プレイヤー位置を基準とした相対位置計算
			Vector3 vec = Normalize(startCameraPos_ - stageManager_->GetPlayer()->GetWorldPosition());
			pos = pos * vec;
			pos = pos + stageManager_->GetPlayer()->GetWorldPosition();

			// 常にプレイヤーを見るように回転を設定
			Vector3 dir = Normalize(stageManager_->GetPlayer()->GetWorldPosition() - pos);

			Vector3 rot;
			rot.y = std::atan2(dir.x, dir.z);
			rot.x = -std::asin(dir.y);
			rot.z = 0.0f;

			// カメラへ適用
			camera_->SetTranslate(pos);
			camera_->SetRotate(rot);
		};


	resetTimer(1.0f);
	resetTimer(2.0f);
	resetTimer(3.0f);
	resetTimer(8.5f);

	// カメラ旋回演出
	if (stateMachine_.GetStateElapsedTime() <= 1.0f)
	{
		Matrix4x4 rotY = MakeRotateYMatrix((startCameraTimer_ - 7.0f) / 3.0f);
		pos = TransformM(pos, rotY);

		applyCameraWork(pos);
	}
	else if (stateMachine_.GetStateElapsedTime() <= 2.0f)
	{
		Matrix4x4 rotY = MakeRotateYMatrix((-startCameraTimer_ + 7.0f) / 3.0f);
		pos = TransformM(pos, rotY);

		applyCameraWork(pos);
	}
	else if (stateMachine_.GetStateElapsedTime() <= 3.0f)
	{
		Matrix4x4 rotX = MakeRotateXMatrix((startCameraTimer_) / 3.0f);
		pos = TransformM(pos, rotX);
		Matrix4x4 rotA = MakeRotateYMatrix(std::numbers::pi_v<float>);
		pos = TransformM(pos, rotA);

		applyCameraWork(pos);
	}
	else if (stateMachine_.GetStateElapsedTime() <= 5.0f)
	{
		if (stateMachine_.GetStateElapsedTime() <= 6.5f)
		{
			startDraw_->Start();
		}
		Matrix4x4 rotX = MakeRotateXMatrix((-startCameraTimer_) / 3.0f);
		pos = TransformM(pos, rotX);

		applyCameraWork(pos);
	}
	else if (stateMachine_.GetStateElapsedTime() <= 8.5f)
	{
	}
	else
	{
		stateMachine_.ChangeState(GameSceneState::PLAY);
	}



#ifdef _DEBUG
	// 個別動作確認用（確認時は↑をコメントアウト）
	if (input_->PushKey(DIK_0))
	{
		// 3
		Matrix4x4 rotX = MakeRotateXMatrix((startCameraTimer_) / 5.0f);
		pos = TransformM(pos, rotX);
		Matrix4x4 rotA = MakeRotateYMatrix(std::numbers::pi_v<float>);
		pos = TransformM(pos, rotA);
	}
	if (input_->PushKey(DIK_MINUS))
	{
		// 4
		Matrix4x4 rotX = MakeRotateXMatrix((-startCameraTimer_) / 5.0f);
		pos = TransformM(pos, rotX);
	}

	// reset
	if (input_->TriggerKey(DIK_7)) startCameraTimer_ = 0;

	if (input_->PushKey(DIK_8))
	{
		// 1
		Matrix4x4 rotY = MakeRotateYMatrix((startCameraTimer_ - 7.0f) / 3.0f);
		pos = TransformM(pos, rotY);
	}
	if (input_->PushKey(DIK_9))
	{
		// 2
		Matrix4x4 rotY = MakeRotateYMatrix((-startCameraTimer_ + 7.0f) / 5.0f);
		pos = TransformM(pos, rotY);
	}

	if (input_->TriggerKey(DIK_SPACE)) stateMachine_.ChangeState(GameSceneState::PLAY);

	ImGui::Begin("StartCamera");
	ImGui::DragFloat3("pos", &pos.x);
	ImGui::End();
#endif // _DEBUG
}