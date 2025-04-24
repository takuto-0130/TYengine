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

/// <summary>
/// ゲームシーン
/// </summary>
class GameScene : public IScene {

public: // メンバ関数

	/// <summary>
	/// デストラクタ
	/// </summary>
	~GameScene() override;

	/// <summary>
	/// 初期化
	/// </summary>
	void Init() override;

	/// <summary>
	/// 毎フレーム処理
	/// </summary>
	void Update() override;

	/// <summary>
	/// 描画
	/// </summary>
	void Draw() override;

	void CheckAllCollisions();

private:
	/// <summary>
/// re-ruのスポーン()
/// </summary>
	void PopRail(Vector3 position, Vector3 rota) {

		std::unique_ptr<Rail> rail = std::make_unique<Rail>();
		rail->Initialize(position);
		rail->SetRotate(rota);
		rail->UpdateTransform();

		rails_.push_back(std::move(rail));
	}

	void RailCustom();

	void RailLineReDraw();

	void RailReDraw();

	void RailCameraMove();

	void RailCameraDebug();

	void SetSegment();

	void ResetRailCamera();

private: // メンバ変数

	float pitch_ = 1.0f;

	Vector3 cameraOffset_;

#ifdef _DEBUG
	bool isEffect_ = false;
#endif // _DEBUG

	// 3Dオブジェクト
	std::unique_ptr<Object3d> obj_;

	// ワールド行列
	WorldTransform worldTransform_;

	std::list<std::unique_ptr<Rail>> rails_;

	std::vector<Vector3> controlPoints_;
	std::vector<Vector3> pointsDrawing_;
	size_t oneSegmentCount = 20;
	size_t segmentCount = oneSegmentCount;

	const float kDivisionSpan = 100.0f;
	float cameraSegmentCount = 1.0f / 600.0f;
	float cameraEyeT = 0;
	float cameraForwardT = 30.0f / 600.0f;

	bool isRailCameraMove_ = false;
};

