#include "Boss.h"
#include "core/Renderer.h"
#include <cmath>

void Boss::Activate()
{
    SetActive(true);
    state_     = EBossState::Entering;
    health_    = maxHealth_;
    fireTimer_ = GameConfig::BOSS_FIRE_INTERVAL_P1;
    moveTime_  = 0.f;
    SetPosition({GameConfig::WORLD_WIDTH * 0.5f, -GetRadius()});
    SetVelocity({0.f, GameConfig::BOSS_SPEED_ENTER});
}

void Boss::Update(float dt, const Vec2& playerPos, EventBus& bus)
{
    if (!IsActive())
        return;

    switch (state_)
    {
    case EBossState::Entering:
    {
        Vec2 p = GetPosition();
        p.y += GetVelocity().y * dt;
        SetPosition(p);
        if (p.y >= GameConfig::BOSS_ENTER_Y)
        {
            SetPosition({p.x, GameConfig::BOSS_ENTER_Y});
            state_ = EBossState::Phase1;
        }
        break;
    }
    case EBossState::Phase1:
    case EBossState::Phase2:
    {
        const bool enraged = (state_ == EBossState::Phase2);
        moveTime_ += dt;

        // 横向正弦摆动（狂暴阶段频率更高 → 更难躲）
        const float amp  = GameConfig::WORLD_WIDTH * 0.5f - GetRadius() - 20.f;
        const float freq = enraged ? 1.6f : 1.0f;
        const float x    = GameConfig::WORLD_WIDTH * 0.5f + amp * std::sin(moveTime_ * freq);
        SetPosition({x, GameConfig::BOSS_ENTER_Y});

        fireTimer_ -= dt;
        const float interval = enraged ? GameConfig::BOSS_FIRE_INTERVAL_P2
                                       : GameConfig::BOSS_FIRE_INTERVAL_P1;
        if (fireTimer_ <= 0.f)
        {
            // 发射请求交给事件总线——Boss 不知道子弹池长什么样
            bus.Emit(BossFireEvent{ GetPosition(), enraged ? 2 : 1, playerPos });
            fireTimer_ = interval;
        }
        break;
    }
    case EBossState::Defeated:
        SetActive(false);
        break;
    }
}

bool Boss::TakeDamage(float dmg)
{
    if (state_ == EBossState::Defeated)
        return false;

    health_ -= dmg;
    if (health_ <= 0.f)
    {
        health_ = 0.f;
        state_  = EBossState::Defeated;
        SetActive(false);
        return true;            // 这一击致死
    }

    // 血量过半 → 永久进入狂暴阶段（只升不降，避免反复横跳）
    if (state_ == EBossState::Phase1 && health_ <= maxHealth_ * 0.5f)
        state_ = EBossState::Phase2;

    return false;
}

void Boss::Draw(Renderer& r) const
{
    r.DrawEntity(GetSprite(), GetPosition(), GetRadius(), GetFallbackColor());
}
