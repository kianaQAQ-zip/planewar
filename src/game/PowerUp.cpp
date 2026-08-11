#include "PowerUp.h"
#include "Player.h"
#include "core/Renderer.h"
#include <cstdlib>

// ---- 三种具体策略 ----
class HealEffect : public IItemEffect
{
public:
    void        Apply(Player& p) const override { p.AddLife(); }
    SpriteId    GetSprite()   const override { return SpriteId::PowerUp; }
    unsigned int GetColor()   const override { return 0x44FF66FF; }   // 绿
};

class RapidFireEffect : public IItemEffect
{
public:
    void        Apply(Player& p) const override { p.UpgradeWeapon(); }
    SpriteId    GetSprite()   const override { return SpriteId::PowerUp; }
    unsigned int GetColor()   const override { return 0xFFCC33FF; }   // 黄
};

class ShieldEffect : public IItemEffect
{
public:
    void        Apply(Player& p) const override { p.AddShield(GameConfig::ITEM_SHIELD_TIME); }
    SpriteId    GetSprite()   const override { return SpriteId::PowerUp; }
    unsigned int GetColor()   const override { return 0x44CCFFFF; }   // 青
};

// 静态常量实例（无状态，可安全被多个道具指针共享）
namespace
{
    const HealEffect      kHeal;
    const RapidFireEffect kRapid;
    const ShieldEffect    kShield;
    const IItemEffect*    kEffects[] = { &kHeal, &kRapid, &kShield };
}

void PowerUp::RandomInit()
{
    const IItemEffect* e = kEffects[std::rand() % 3];
    effect_ = e;
    SetSprite(e->GetSprite());
    SetFallbackColor(e->GetColor());
}

void PowerUp::Update(float dt)
{
    SetPosition(GetPosition() + GetVelocity() * dt);
    if (GetPosition().y > GameConfig::WORLD_HEIGHT + GetRadius())
        SetActive(false);   // 漏接则回收
}

void PowerUp::Draw(Renderer& r) const
{
    r.DrawEntity(GetSprite(), GetPosition(), GetRadius(), GetFallbackColor());
}
