#pragma once
#include "BaseObject.h"

// blenderで作成したレベルエディタの配置オブジェクトを受け取るクラス
class LevelObject :
    public BaseObject
{
public:
    void Init() override;

    void Update()override;

    void Draw()override;

public:
    void SetModelName(const std::string& modelName) { modelName_ = modelName; }
    void SetPosition(const Vector3& pos) { worldTransform_.SetTranslation(pos); }
    void SetRotation(const Vector3& rotate) { worldTransform_.SetRotate(rotate); }
    void SetScale(const Vector3& scale) { worldTransform_.SetScale(scale); }

private:
    std::string modelName_;
};

