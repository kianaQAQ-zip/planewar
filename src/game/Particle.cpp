#include "Particle.h"
#include "core/Renderer.h"
#include <cmath>

void Particle::Init(const Vec2& pos, const Vec2& vel, float life, unsigned int color)
{
    SetPosition(pos);
    SetVelocity(vel);
    life_    = life;
    maxLife_ = life;
    color_   = color;
    SetActive(true);
    SetRadius(3.f);
}

void Particle::Update(float dt)
{
    SetPosition(GetPosition() + GetVelocity() * dt);
    SetVelocity(GetVelocity() * 0.92f);   // 阻尼，模拟碎片减速
    life_ -= dt;
    if (life_ <= 0.f)
        SetActive(false);
}

void Particle::Draw(Renderer& r) const
{
    const float ratio = maxLife_ > 0.f ? std::max(0.f, life_ / maxLife_) : 0.f;
    const unsigned int alpha = static_cast<unsigned int>(255.f * ratio) & 0xFF;
    const unsigned int c = (color_ & 0xFFFFFF00) | alpha;   // 保留 RGB，覆盖 Alpha（0xRRGGBBAA）
    r.DrawCircle(GetPosition(), GetRadius() * (0.4f + 0.6f * ratio), c);
}
