#include "Reticle.h"
#include "ColliderManager.h"
#include "../../ColliderTypeID/ColliderTypeID.h"
#include "Camera.h"
#include "Input.h"

Reticle::~Reticle()
{
	ColliderManager::GetInstance()->RemoveCollider(collider_.get());
}

void Reticle::Init()
{
	input_ = Input::GetInstance();
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
	if (frameDistance_ != 0.0f)
	{
		targetDistance_ = frameDistance_;
	}
	frameDistance_ = 0.0f;
	
	ScreenToWorld();
}

void Reticle::Draw()
{

}

void Reticle::ScreenToWorld()
{
	Vector2 relative = input_->GetMousePositionRelative();
	Vector2 ndc = {
		relative.x * 2.0f - 1.0f,
		-(relative.y * 2.0f - 1.0f)
	};

	Matrix4x4 invViewProj = Inverse(camera_->GetViewProjectionMatrix());

	Vector3 ndcFar = { ndc.x, ndc.y, 1.0f };
	Vector3 worldFar = TransformM(ndcFar, invViewProj);

	Vector3 rayOrigin = camera_->GetPosition();
	Vector3 rayDir = Normalize(Vector3{
		worldFar.x - rayOrigin.x,
		worldFar.y - rayOrigin.y,
		worldFar.z - rayOrigin.z
		});

	collider_->SetDirection(rayDir);
	collider_->Update(rayOrigin);
}
