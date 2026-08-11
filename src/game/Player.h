#pragma once
#include "Entity.h"
#include "ObjectPool.h"
#include "Input.h"
#include "GameConfig.h"
#include "Bullet.h"
#include <cmath>

// 玩家飞机。非池化（全场只有一个），由 World 直接持有。
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
        lives_ = GameConfig::PLAYER_LIVES;
        fireCooldown_ = 0.f;
    }

    // 每帧由 World 调用：把输入转成速度，并在开火冷却就绪时从子弹池借一颗子弹。
    void HandleInput(const InputState& input, ObjectPool<Bullet>& bullets);

    void Update(float dt) override;
    void Draw(Renderer& r) const override;

    int  GetLives() const { return lives_; }
    bool IsDead()   const { return lives_ <= 0; }
    void LoseLife()       { if (lives_ > 0) --lives_; }

private:
    float fireCooldown_ = 0.f;                 // 距下次可开火的剩余秒数
    int   lives_        = GameConfig::PLAYER_LIVES;
};
