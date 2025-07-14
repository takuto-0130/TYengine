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
		Vector3(0, 0, 0)
	);
	ColliderManager::GetInstance()->AddCollider(collider_.get());
}

void Reticle::Update()
{

}

void Reticle::Draw()
{

}

void Reticle::ScreenToWorld()
{
	//Vector2 relative = input_->GetMousePositionRelative(); // 0〜1
	//Vector2 ndc = {
	//	relative.x * 2.0f - 1.0f,
	//	-(relative.y * 2.0f - 1.0f)
	//};
}
