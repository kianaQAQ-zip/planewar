#pragma once
#include "core/Math.h"
#include "core/SpriteId.h"

class Renderer;   // 前向声明：Game 层头文件不引入 SFML，仅在 .cpp 里通过 Core 的 Renderer 绘制

// 所有游戏对象的基类（Game 层）。
// 只持有与渲染/逻辑无关的「数据」：位置、速度、碰撞半径、要画的精灵、激活状态、缺图兜底色。
// 不直接持有任何 SFML 对象——怎么画交给 Core 的 Renderer。
class Entity
{
public:
    Entity() = default;
    virtual ~Entity() = default;

    // 允许对象池按值存放（vector<T> 需要可拷贝/移动）。
    Entity(const Entity&) = default;
    Entity& operator=(const Entity&) = default;

    virtual void Update(float dt) {}
    virtual void Draw(Renderer& renderer) const {}
    virtual void OnSpawn() {}                 // 被对象池 Acquire 时调用，用于重置状态

    void SetActive(bool a) { active_ = a; }
    bool IsActive() const { return active_; }

    void SetPosition(const Vec2& p) { pos_ = p; }
    Vec2 GetPosition() const        { return pos_; }
    void SetVelocity(const Vec2& v) { vel_ = v; }
    Vec2 GetVelocity() const        { return vel_; }

    void SetRadius(float r)  { radius_ = r; }
    float GetRadius() const  { return radius_; }

    void SetSprite(SpriteId s) { sprite_ = s; }
    SpriteId GetSprite() const { return sprite_; }

    void SetFallbackColor(unsigned int c) { fallback_ = c; }
    unsigned int GetFallbackColor() const { return fallback_; }

protected:
    Vec2       pos_{};
    Vec2       vel_{};
    float      radius_   = 16.f;
    SpriteId   sprite_   = SpriteId::None;
    unsigned int fallback_ = 0xFFFFFFFF;       // 缺纹理时的占位圆颜色（默认白）
    bool       active_   = false;
};
