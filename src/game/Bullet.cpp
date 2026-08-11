#include "Bullet.h"
#include "core/Renderer.h"

void Bullet::Update(float dt)
{
    SetPosition(GetPosition() + GetVelocity() * dt);
    // 飞出屏幕顶部即回收（自行 SetActive(false)，对象池下一帧可复用）
    if (GetPosition().y < -GetRadius())
        SetActive(false);
}

void Bullet::Draw(Renderer& r) const
{
    r.DrawEntity(GetSprite(), GetPosition(), GetRadius(), GetFallbackColor());
}
