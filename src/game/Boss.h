#pragma once
#include "Entity.h"
#include "events/EventBus.h"
#include "GameConfig.h"

// Boss（设计模式：状态机 / State）
// ---------------------------------------------------------------------------
// 用显式状态枚举 + 每状态独立的更新分支，替代「if (state == ...) 地狱」。
// 新增阶段（比如 Phase3）只需加一个枚举值 + 一段 case，旧逻辑一行不用改——这就是
// 状态机相对分支的好处，也是面试讲「开闭原则」的现成例子。
enum class EBossState
{
    Entering,   // 从屏幕顶端进场，降到停驻高度
    Phase1,     // 常规弹幕
    Phase2,     // 血量过半狂暴：移速更快、弹幕更密
    Defeated    // 被击毁（最后一击触发）
};

class Boss : public Entity
{
public:
    Boss()
    {
        SetSprite(SpriteId::Boss);
        SetRadius(GameConfig::BOSS_RADIUS);
        SetFallbackColor(0xCC33CCFF);   // 紫色占位
        SetActive(false);
    }

    void Activate();                    // 进场（World 在波次清空后调用一次）
    void Deactivate() { SetActive(false); }

    // 每帧推进。playerPos 用于后续瞄准弹幕；发射通过 Emit(BossFireEvent) 解耦给 World。
    void Update(float dt, const Vec2& playerPos, EventBus& bus);

    // 命中扣血。返回 true 表示「这一击把它打死了」（供 World 触发胜利/大爆炸）。
    bool TakeDamage(float dmg);

    bool  IsDefeated()    const { return state_ == EBossState::Defeated; }
    float GetHealthRatio() const { return maxHealth_ > 0.f ? health_ / maxHealth_ : 0.f; }

    void Draw(Renderer& r) const override;

private:
    EBossState state_     = EBossState::Entering;
    float      health_    = GameConfig::BOSS_MAX_HP;
    float      maxHealth_ = GameConfig::BOSS_MAX_HP;
    float      fireTimer_ = 0.f;
    float      moveTime_  = 0.f;
};
