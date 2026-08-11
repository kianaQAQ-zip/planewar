#include "Enemy.h"
#include "core/Renderer.h"

void Enemy::Update(float dt)
{
    SetPosition(GetPosition() + GetVelocity() * dt);
    // 飞出屏幕底部即回收
    if (GetPosition().y > GameConfig::WORLD_HEIGHT + GetRadius())
        SetActive(false);
}

void Enemy::Draw(Renderer& r) const
{
    r.DrawEntity(GetSprite(), GetPosition(), GetRadius(), GetFallbackColor());
}
