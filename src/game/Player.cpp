#include "Player.h"
#include "core/Renderer.h"
#include <algorithm>

void Player::HandleInput(const InputState& input, ObjectPool<Bullet>& bullets)
{
    Vec2 dir{0.f, 0.f};
    if (input.left)  dir.x -= 1.f;
    if (input.right) dir.x += 1.f;
    if (input.up)    dir.y -= 1.f;
    if (input.down)  dir.y += 1.f;

    // 八方向归一化，避免斜向移动更快
    if (dir.x != 0.f && dir.y != 0.f)
    {
        const float inv = 1.f / std::sqrt(2.f);
        dir.x *= inv;
        dir.y *= inv;
    }
    SetVelocity(dir * GameConfig::PLAYER_SPEED);

    if (input.fire && fireCooldown_ <= 0.f)
    {
        if (Bullet* b = bullets.Acquire())
        {
            // 从机头上方出生
            b->SetPosition({GetPosition().x, GetPosition().y - GetRadius()});
            fireCooldown_ = GameConfig::PLAYER_FIRE_RATE;
        }
    }
}

void Player::Update(float dt)
{
    SetPosition(GetPosition() + GetVelocity() * dt);

    // 限制在屏幕内
    const float r = GetRadius();
    Vec2 p = GetPosition();
    p.x = std::clamp(p.x, r, GameConfig::WORLD_WIDTH  - r);
    p.y = std::clamp(p.y, r, GameConfig::WORLD_HEIGHT - r);
    SetPosition(p);

    if (fireCooldown_ > 0.f)
        fireCooldown_ -= dt;
}

void Player::Draw(Renderer& r) const
{
    r.DrawEntity(GetSprite(), GetPosition(), GetRadius(), GetFallbackColor());
}
