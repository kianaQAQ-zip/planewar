#include "Bullet.h"
#include "core/Renderer.h"

void Bullet::Update(float dt)
{
    SetPosition(GetPosition() + GetVelocity() * dt);
    // 玩家子弹向上飞出顶边、敌弹向下/侧向飞出，都在此统一回收（对象池下一帧可复用）。
    const float r = GetRadius();
    const Vec2 p = GetPosition();
    if (p.y < -r || p.y > GameConfig::WORLD_HEIGHT + r ||
        p.x < -r || p.x > GameConfig::WORLD_WIDTH + r)
        SetActive(false);
}

void Bullet::Draw(Renderer& r) const
{
    r.DrawEntity(GetSprite(), GetPosition(), GetRadius(), GetFallbackColor());
}
