#pragma once
#include "Entity.h"
#include "GameConfig.h"

// 玩家子弹。池化对象：World 持有 ObjectPool<Bullet>，平时休眠，开火时 Acquire。
class Bullet : public Entity
{
public:
    Bullet()
    {
        SetSprite(SpriteId::BulletPlayer);
        SetRadius(GameConfig::BULLET_RADIUS);
        SetFallbackColor(0xFFFF33FF);   // 黄色占位
    }

    void OnSpawn() override
    {
        // 速度由 OnSpawn 设定（对象复用时会重置），位置由 World 在 Acquire 后填入。
        SetVelocity({0.f, -GameConfig::BULLET_SPEED});
    }

    void Update(float dt) override;
    void Draw(Renderer& r) const override;
};
