#pragma once
#include <cmath>

// 轻量 2D 向量。放在 Core 层，Game 层用它可以完全不碰 SFML 的 sf::Vector2f，
// 从而守住「Game 层不引用 SFML 类型」的架构纪律。Renderer 在绘制时再转成 sf::Vector2f。
struct Vec2
{
    float x = 0.f;
    float y = 0.f;

    Vec2() = default;
    Vec2(float x_, float y_) : x(x_), y(y_) {}

    Vec2 operator+(const Vec2& o) const { return {x + o.x, y + o.y}; }
    Vec2 operator-(const Vec2& o) const { return {x - o.x, y - o.y}; }
    Vec2 operator*(float s) const       { return {x * s,   y * s}; }

    Vec2& operator+=(const Vec2& o) { x += o.x; y += o.y; return *this; }

    float LengthSq() const { return x * x + y * y; }
    float Length()   const { return std::sqrt(LengthSq()); }
};

inline float DistanceSq(const Vec2& a, const Vec2& b)
{
    const float dx = a.x - b.x;
    const float dy = a.y - b.y;
    return dx * dx + dy * dy;
}
