#pragma once
#include "Entity.h"
#include "ObjectPool.h"
#include "Input.h"
#include "GameConfig.h"
#include "Bullet.h"
#include <cmath>

// 玩家飞机。非池化（全场只有一个），由 World 直接持有。
// M3 增强：武器等级（多发）、护盾、受击短无敌——这些状态全部内聚在 Player，
// 由道具策略（PowerUp）通过 AddLife/UpgradeWeapon/AddShield 间接修改，互不耦合。
class Player : public Entity
{
public:
    Player()
    {
        SetSprite(SpriteId::Player);
        SetRadius(GameConfig::PLAYER_RADIUS);
        SetFallbackColor(0x44CCFFFF);   // 青色占位
    }

    void Reset(const Vec2& spawn)
    {
        SetPosition(spawn);
        SetVelocity({0.f, 0.f});
        SetActive(true);
        lives_       = GameConfig::PLAYER_LIVES;
        fireCooldown_ = 0.f;
        weaponLevel_  = 0;
        invulnTimer_  = 0.f;
        shieldTimer_  = 0.f;
    }

    // 每帧由 World 调用：把输入转成速度，并在开火冷却就绪时从子弹池借子弹。
    // 返回是否「本帧开火」，供 World 触发音效/枪口粒子（避免每帧误报）。
    bool HandleInput(const InputState& input, ObjectPool<Bullet>& bullets);

    void Update(float dt) override;
    void Draw(Renderer& r) const override;

    int  GetLives() const { return lives_; }
    bool IsDead()   const { return lives_ <= 0; }
    void LoseLife()       { if (lives_ > 0) --lives_; invulnTimer_ = GameConfig::PLAYER_HIT_INVULN; }

    // 道具效果调用的接口（策略模式：效果对象只认这些）
    void AddLife()          { if (lives_ < GameConfig::PLAYER_LIVES) ++lives_; }
    void UpgradeWeapon()    { if (weaponLevel_ < 2) ++weaponLevel_; }
    void AddShield(float s) { shieldTimer_ = s; }

    int  GetWeaponLevel() const { return weaponLevel_; }
    bool IsShielded()    const { return shieldTimer_ > 0.f; }
    // 是否可被击中：无敌帧未过 或 处于护盾中 → 免伤（同时吸收子弹/碰撞而不扣命）
    bool CanBeHit()      const { return invulnTimer_ <= 0.f && !IsShielded(); }

private:
    float fireCooldown_ = 0.f;                 // 距下次可开火的剩余秒数
    int   lives_        = GameConfig::PLAYER_LIVES;
    int   weaponLevel_  = 0;                   // 0→单发, 1→双发, 2→三发
    float invulnTimer_  = 0.f;                 // 受击后短暂无敌
    float shieldTimer_  = 0.f;                 // 护盾剩余秒数
};
