#pragma once
#include "Entity.h"
#include "GameConfig.h"

// 基础敌机。池化对象：World 定时 Acquire 生成。
class Enemy : public Entity
{
public:
    Enemy()
    {
        SetSprite(SpriteId::EnemyBasic);
        SetRadius(GameConfig::ENEMY_RADIUS);
        SetFallbackColor(0xFF4444FF);   // 红色占位
    }

    void OnSpawn() override
    {
        // 速度由 OnSpawn 设定；出生位置由 World 在 Acquire 后填入（屏幕顶部随机 x）。
        SetVelocity({0.f, GameConfig::ENEMY_SPEED});
    }

    void Update(float dt) override;
    void Draw(Renderer& r) const override;
};
