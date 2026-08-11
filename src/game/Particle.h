#pragma once
#include "Entity.h"
#include "GameConfig.h"

// 粒子（爆炸碎片）。池化对象：爆炸瞬间批量 Acquire，生命耗尽自动回收。
// 颜色自带 Alpha，绘制时按剩余寿命淡出——但淡出用「缩小 + 降透明度」实现，
// 不引入额外纹理，热路径零分配。
class Particle : public Entity
{
public:
    Particle() { SetSprite(SpriteId::None); SetRadius(3.f); }

    void OnSpawn() override {}

    // 一次性初始化（位置/速度/寿命/颜色）。寿命按比例决定透明度与大小。
    void Init(const Vec2& pos, const Vec2& vel, float life, unsigned int color);

    void Update(float dt) override;
    void Draw(Renderer& r) const override;

private:
    float        life_    = 0.f;
    float        maxLife_ = 1.f;
    unsigned int color_   = 0xFFFFFFFF;
};
