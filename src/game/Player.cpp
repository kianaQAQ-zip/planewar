#include "Player.h"
#include "core/Renderer.h"
#include <algorithm>

bool Player::HandleInput(const InputState& input, ObjectPool<Bullet>& bullets)
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

    bool fired = false;
    if (input.fire && fireCooldown_ <= 0.f)
    {
        const int   shots  = 1 + weaponLevel_;     // 武器等级决定弹数
        const float spread = 14.f;
        for (int i = 0; i < shots; ++i)
        {
            if (Bullet* b = bullets.Acquire())
            {
                // 多发沿 x 轴小幅错开，形成散射
                const float offset = (i - (shots - 1) * 0.5f) * spread;
                b->SetPosition({GetPosition().x + offset, GetPosition().y - GetRadius()});
                fired = true;
            }
        }
        // 武器等级越高射速越快（向下取整到 0.08s 上限）
        const float rate = std::max(0.08f,
            GameConfig::PLAYER_FIRE_RATE * (1.f - 0.18f * weaponLevel_));
        fireCooldown_ = rate;
    }
    return fired;
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

    if (fireCooldown_ > 0.f) fireCooldown_ -= dt;
    if (invulnTimer_  > 0.f) invulnTimer_  -= dt;
    if (shieldTimer_  > 0.f) shieldTimer_  -= dt;
}

void Player::Draw(Renderer& r) const
{
    r.DrawEntity(GetSprite(), GetPosition(), GetRadius(), GetFallbackColor());
    // 护盾期画一层外圈，给玩家明确的视觉反馈（复用 Core 的 DrawCircle）
    if (IsShielded())
        r.DrawCircle(GetPosition(), GetRadius() + 6.f, 0x44CCFF88);
}
