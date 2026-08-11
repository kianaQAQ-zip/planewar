#pragma once
#include "Entity.h"
#include "GameConfig.h"

class Player;   // 前向声明：效果在 .cpp 中作用于 Player，避免 Game 层交叉包含

// 道具效果（设计模式：策略 / Strategy）
// ---------------------------------------------------------------------------
// 不同道具 = 不同「效果对象」，运行时可替换，拾取时调用 Apply(Player&) 即可，
// 容器无需写「if (kind == Heal) ... else if (kind == Shield) ...」那种分支。
// 关键点：三种效果都是无状态的，所以 PowerUp 只持有一个「非拥有」的常量指针，
// 既能用上策略模式，又保证了对象池要求实体「可拷贝」（unique_ptr 会导致不可拷贝）。
class IItemEffect
{
public:
    virtual ~IItemEffect() = default;
    virtual void      Apply(Player& p) const = 0;
    virtual SpriteId  GetSprite()     const = 0;
    virtual unsigned int GetColor()   const = 0;
};

class PowerUp : public Entity
{
public:
    PowerUp() { SetRadius(GameConfig::ITEM_RADIUS); }

    void OnSpawn() override {}                 // 池化要求；效果在 RandomInit 里挑

    // 随机挑一种效果，并据此设置精灵与兜底色（缺图时也能靠颜色区分三类道具）。
    void RandomInit();

    void Update(float dt) override;
    void Draw(Renderer& r) const override;

    // 玩家吃到时执行效果（委托给当前策略对象）。
    void ApplyTo(Player& p) const { if (effect_) effect_->Apply(p); }

private:
    const IItemEffect* effect_ = nullptr;      // 指向静态实例，不拥有
};
