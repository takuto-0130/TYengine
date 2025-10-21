#include "../GameScene.h"
#include "Timer.h"
#ifdef _DEBUG
#include "imgui.h"
#endif // _DEBUG


void GameScene::InitReady()
{
	startCameraTimer_ = 0;
}
void GameScene::UpdateReady()
{
	startCameraTimer_ += Timer::GetInstance()->GetDeltaTime();
	StartCamera();
}
void GameScene::ExitReady()
{
	startCameraTimer_ = 0;
}





void GameScene::StartCamera()
{
	Vector3 pos = { 0,0,7 };


	// 演出タイマーのリセット用
	std::function<void(float)> resetTimer = [&](float threshold)
		{
			if (GetStateElapsedTime() + Timer::GetInstance()->GetDeltaTime() > threshold)
			{
				startCameraTimer_ = 0;
			}
		};

	// カメラワークを適用
	std::function<void(Vector3&)> applyCameraWork = [&](Vector3& pos)
		{
			// 真後ろから始まるように
			Matrix4x4 rotAjust = MakeRotateYMatrix((3.9f) / 5.0f);
			pos = TransformM(pos, rotAjust);


			Vector3 vec = Normalize(startCameraPos_ - stageManager_->GetPlayer()->GetWorldPosition());
			pos = pos * vec;
			pos = pos + stageManager_->GetPlayer()->GetWorldPosition();

			Vector3 dir = Normalize(stageManager_->GetPlayer()->GetWorldPosition() - pos);

			Vector3 rot;
			rot.y = std::atan2(dir.x, dir.z);
			rot.x = -std::asin(dir.y);
			rot.z = 0.0f;


			camera_->SetTranslate(pos);
			camera_->SetRotate(rot);
		};


	if (GetStateElapsedTime() <= 3.0f)
	{
		Matrix4x4 rotY = MakeRotateYMatrix((startCameraTimer_ - 7.0f) / 3.0f);
		pos = TransformM(pos, rotY);

		applyCameraWork(pos);
		resetTimer(3.0f);
	}
	else if (GetStateElapsedTime() <= 6.0f)
	{
		Matrix4x4 rotY = MakeRotateYMatrix((-startCameraTimer_ + 7.0f) / 3.0f);
		pos = TransformM(pos, rotY);

		applyCameraWork(pos);
		resetTimer(6.0f);
	}
	else if (GetStateElapsedTime() <= 10.0f)
	{
		Matrix4x4 rotX = MakeRotateXMatrix((startCameraTimer_) / 5.0f);
		pos = TransformM(pos, rotX);
		Matrix4x4 rotA = MakeRotateYMatrix(std::numbers::pi_v<float>);
		pos = TransformM(pos, rotA);

		applyCameraWork(pos);
		resetTimer(10.0f);
	}
	else if (GetStateElapsedTime() <= 13.0f)
	{
		Matrix4x4 rotX = MakeRotateXMatrix((-startCameraTimer_) / 5.0f);
		pos = TransformM(pos, rotX);

		applyCameraWork(pos);
		resetTimer(13.0f);
	}
	else
	{
		ChangeState(GameSceneState::PLAY);
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

	if (input_->TriggerKey(DIK_SPACE)) ChangeState(GameSceneState::PLAY);

	ImGui::Begin("StartCamera");
	ImGui::DragFloat3("pos", &pos.x);
	ImGui::End();
#endif // _DEBUG
}