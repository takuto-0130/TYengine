#pragma once
#include "BaseObject.h"

class Rail : public BaseObject
{
public:

	~Rail();

	void Init();

	void Update() override;

	void Draw() override;

	void SetRotate(const Vector3& rotation) { worldTransform_.rotation_ = rotation; }
	void SetTranslation(const Vector3& translation) { worldTransform_.translation_ = translation; }
};
