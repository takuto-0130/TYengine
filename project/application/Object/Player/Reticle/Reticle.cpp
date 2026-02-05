#include "Reticle.h"
#include "ColliderManager.h"
#include "../../ColliderTypeID/ColliderTypeID.h"
#include "Camera.h"
#include "Input.h"

using namespace TYEngine::Utility;
using namespace TYEngine;

Reticle::~Reticle()
{
	ColliderManager::GetInstance()->RemoveCollider(collider_.get());
}

void Reticle::Init()
{
	input_ = Framework::Input::GetInstance();
	
	// レティクル用レイコライダーの生成
	collider_ = std::make_unique<ReticleCollider>(
		static_cast<uint32_t>(ColliderTypeID::RETICLE),
		Vector3(0, 0, 0),
		Vector3(0, 0, 0),
		this
	);
	ColliderManager::GetInstance()->AddCollider(collider_.get());

	targetDistance_ = defaultDistance_;
}

void Reticle::Update()
{
	// ターゲットとの距離更新（毎フレームリセットされるため、衝突通知があれば更新）
	if (frameDistance_ != 0.0f)
	{
		targetDistance_ = frameDistance_;
	}
	frameDistance_ = 0.0f;
	
	// マウス位置に基づくワールドレイ情報の更新
	ScreenToWorld();
}

void Reticle::Draw()
{

}

void Reticle::ScreenToWorld()
{
	// マウス座標（スクリーン座標）を取得しNDCへ変換
	Vector2 relative = input_->GetMousePositionRelative();
	Vector2 ndc = {
		relative.x * 2.0f - 1.0f,
		-(relative.y * 2.0f - 1.0f)
	};

	// ビュープロジェクション行列の逆行列を使用してワールド座標を算出
	Matrix4x4 invViewProj = Inverse(camera_->GetViewProjectionMatrix());

	Vector3 ndcFar = { ndc.x, ndc.y, 1.0f };
	Vector3 worldFar = TransformM(ndcFar, invViewProj);

	// カメラ位置からクリック地点遠方へのレイベクトルを計算
	Vector3 rayOrigin = camera_->GetPosition();
	Vector3 rayDir = Normalize(Vector3{
		worldFar.x - rayOrigin.x,
		worldFar.y - rayOrigin.y,
		worldFar.z - rayOrigin.z
		});

	// コライダーへレイ情報を適用
	collider_->SetDirection(rayDir);
	collider_->Update(rayOrigin);
}
