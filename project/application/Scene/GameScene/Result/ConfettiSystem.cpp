#include "ConfettiSystem.h"
#include "Random.h"
#include "Ease.h"
#include <algorithm>
#include <cmath>

void ConfettiSystem::Init(int maxCount, const std::string& texturePath,
    int screenW, int screenH)
{
    screenW_ = screenW;
    screenH_ = screenH;
    texturePath_ = texturePath;

    particles_.resize(maxCount);

    for (auto& p : particles_)
    {
        p.active = false;
        p.sprite.Initialize(texturePath_);
        p.sprite.SetAnchorPoint({ 0.5f, 0.5f });
    }
}

void ConfettiSystem::Burst(int count)
{
    for (int i = 0; i < count; ++i)
    {
        SpawnOne();
    }
}

void ConfettiSystem::Emit(int countPerFrame)
{
    for (int i = 0; i < countPerFrame; ++i)
    {
        SpawnOne();
    }
}

void ConfettiSystem::Update(float dt)
{
    globalTime_ += dt;

    const float gravity = 300.0f;

    for (auto& p : particles_)
    {
        if (!p.active)
        {
            continue;
        }

        // 風揺れ用の時間
        p.swayTime += dt;

        // 落下（軽さに応じてスケール）
        p.vel.y += gravity * p.fallScale * dt;

        // 位置更新（基準位置）
        p.pos.x += p.vel.x * dt;
        p.pos.y += p.vel.y * dt;

        // 回転
        p.angle += p.angularVel * dt;

        // 寿命減算
        p.life -= dt;

        // 画面外または寿命尽きたら非アクティブ化
        if (p.life <= 0.0f || p.pos.y > screenH_ + 50.0f)
        {
            p.active = false;
            continue;
        }

        // 生存率 0〜1 based on life
        float t = p.life / p.maxLife;
        t = std::clamp(t, 0.0f, 1.0f);

        // フワっと消えるα (イージング)
        float alpha = EaseFixed::OutQuad(t);

        // 風の左右揺れ：sin波でオフセット加算
        float swayX = std::sin(p.swayTime * p.swayFreq + p.swayPhase) * p.swayAmp;

        // 実際に描画する位置（基準 + 風オフセット）
        Vector2 drawPos = { p.pos.x + swayX, p.pos.y };

        // UI と衝突チェック → 当たったら分裂して消える
        if (CheckAndHandleHitUI(p, drawPos))
        {
            continue; // このパーティクルは消えているので次へ
        }

        p.sprite.SetPosition(drawPos);
        p.sprite.SetRotation(p.angle);
        p.sprite.SetAlpha(alpha);

        p.sprite.Update();
    }
}

void ConfettiSystem::Draw()
{
    // 奥行きレイヤー順にソート（奥 → 手前）
    std::sort(particles_.begin(), particles_.end(),
        [](const ConfettiParticle& a, const ConfettiParticle& b)
        {
            return a.depth < b.depth;
        });

    for (auto& p : particles_)
    {
        if (!p.active)
        {
            continue;
        }
        p.sprite.Draw();
    }
}

void ConfettiSystem::SpawnOne()
{
    // 非アクティブなスロットを探す
    auto it = std::find_if(particles_.begin(), particles_.end(),
        [](const ConfettiParticle& pt) { return !pt.active; });
    if (it == particles_.end())
    {
        return;
    }

    ConfettiParticle& p = *it;
    auto* rnd = Random::GetInstance();

    // -----------------------------
    // Z順序（奥行きレイヤー）
    // 0.0 = 奥、1.0 = 手前
    // -----------------------------
    p.depth = rnd->Float(0.0f, 1.0f);

    // 奥は小さく / 手前は大きく
    auto Lerp = [](float a, float b, float t)
        {
            return a + (b - a) * t;
        };

    // 落下スケール（軽いほどふわっと）
    p.fallScale = Lerp(0.4f, 1.0f, p.depth); // 奥(0.4) → 手前(1.0) のイメージ

    // -----------------------------
    // 位置・速度（「軽い」ほどゆっくり）
    // -----------------------------
    p.pos.x = rnd->Float(0.0f, (float)screenW_);
    p.pos.y = rnd->Float(-150.0f, -50.0f);

    // 横速度：左右に適当に
    p.vel.x = rnd->Float(-40.0f, 40.0f) * p.fallScale;

    // 縦速度：小さめ＋fallScaleで調整 → フワッと落ちる
    p.vel.y = rnd->Float(20.0f, 80.0f) * p.fallScale;

    // -----------------------------
    // 回転（ランダムな倍率）
    // -----------------------------
    float spinBase = rnd->Float(-1.0f, 1.0f);   // 回転方向
    float spinMul = rnd->Float(0.5f, 4.0f);    // 「クルクル度」倍率
    p.angularVel = spinBase * spinMul;
    p.angle = rnd->Float(0.0f, 6.28318f);

    // -----------------------------
    // 寿命
    // -----------------------------
    p.maxLife = rnd->Float(2.0f, 4.0f);
    p.life = p.maxLife;

    // -----------------------------
    // サイズ（2x2白テクスチャ前提）
    // 奥は小さく・手前は大きく
    // -----------------------------
    float baseWidth = rnd->Float(6.0f, 20.0f);
    float baseHeight = rnd->Float(3.0f, 10.0f);

    float sizeScale = Lerp(0.6f, 1.4f, p.depth); // 奥:0.6 / 手前:1.4
    float width = baseWidth * sizeScale;
    float height = baseHeight * sizeScale;

    p.sprite.SetSize({ width, height });

    // -----------------------------
    // 風で左右にゆらぐパラメータ
    // -----------------------------
    p.swayTime = 0.0f;
    p.swayPhase = rnd->Float(0.0f, 6.28318f);   // 初期位相
    p.swayFreq = rnd->Float(1.0f, 3.0f);       // 周波数（早さ）
    p.swayAmp = rnd->Float(8.0f, 32.0f) * p.fallScale; // 振幅（軽いほどよく揺れる）

    // -----------------------------
    // カラー（2x2白テクスチャだから色は自由）
    // -----------------------------
    static const Vector4 colors[] = {
        {1.0f, 0.4f, 0.4f, 1.0f}, // 赤
        {0.4f, 1.0f, 0.4f, 1.0f}, // 緑
        {0.4f, 0.6f, 1.0f, 1.0f}, // 青
        {1.0f, 1.0f, 0.4f, 1.0f}, // 黄
        {1.0f, 0.6f, 1.0f, 1.0f}, // ピンク
    };
    int idx = rnd->Int(0, 4);
    p.sprite.SetColor(colors[idx]);

    p.active = true;

    p.isShatter = false;
}

static AABB2D GetConfettiAABB(ConfettiParticle& p, const Vector2& drawPos)
{
    Vector2 size = p.sprite.GetSize();
    Vector2 anchor = p.sprite.GetAnchorPoint();

    float left = drawPos.x - size.x * anchor.x;
    float top = drawPos.y - size.y * anchor.y;
    float right = left + size.x;
    float bottom = top + size.y;

    AABB2D box;
    box.min = { left,  top };
    box.max = { right, bottom };
    return box;
}

bool ConfettiSystem::CheckAndHandleHitUI(ConfettiParticle& p, const Vector2& drawPos)
{
    if (p.isShatter) return false;

    if (!uiAABBs_) return false;

    AABB2D confettiBox = GetConfettiAABB(p, drawPos);

    for (const auto& uiBox : *uiAABBs_)
    {
        if (IntersectAABB(confettiBox, uiBox))
        {
            // 当たったら紙吹雪を消して「散らす」
            Vector4 color = p.sprite.GetColor();
            float   depth = p.depth;
            p.active = false;

            // 散る紙吹雪を複数生成
            SpawnShatter(drawPos, color, depth, 5); // 5枚くらいに分裂

            return true;
        }
    }
    return false;
}

void ConfettiSystem::SpawnShatter(const Vector2& pos, const Vector4& color, float depth, int count)
{
    auto* rnd = Random::GetInstance();

    for (int i = 0; i < count; ++i)
    {
        // 空きスロット探す
        auto it = std::find_if(particles_.begin(), particles_.end(),
            [](const ConfettiParticle& pt) { return !pt.active; });
        if (it == particles_.end())
        {
            return;
        }

        ConfettiParticle& p = *it;

        p.active = true;
        p.isShatter = true; // 破片フラグON
        p.depth = depth; // 元のレイヤー継承

        p.pos = pos;

        // ランダムな方向に飛ばす
        /*float angle = rnd->Float(0.0f, 6.28318f);
        float speed = rnd->Float(80.0f, 200.0f);

        p.vel.x = std::cos(angle) * speed;
        p.vel.y = std::sin(angle) * speed;*/

        // 上方向にだけ跳ねるようにする
        float speedUp = rnd->Float(100.0f, 200.0f);         // 上への基本速度
        float sideRate = rnd->Float(-0.6f, 0.6f);            // 左右への振り（-0.6〜0.6倍）

        p.vel.x = speedUp * sideRate;   // 左右は少しだけ
        p.vel.y = -speedUp;             // 上方向（画面座標だとマイナス）

        // 回転も少し大きめ
        p.angularVel = rnd->Float(-6.0f, 6.0f);
        p.angle = rnd->Float(0.0f, 6.28318f);

        // 寿命短め
        p.maxLife = rnd->Float(0.4f, 0.8f);
        p.life = p.maxLife;

        // 小さめの破片
        float w = rnd->Float(3.0f, 8.0f);
        float h = rnd->Float(2.0f, 5.0f);
        p.sprite.SetSize({ w, h });

        p.sprite.SetColor(color); // 元の色を継承

        // 風揺れは弱めに
        p.fallScale = 1.0f;
        p.swayTime = 0.0f;
        p.swayPhase = rnd->Float(0.0f, 6.28318f);
        p.swayFreq = rnd->Float(2.0f, 4.0f);
        p.swayAmp = rnd->Float(4.0f, 12.0f);
    }
}
